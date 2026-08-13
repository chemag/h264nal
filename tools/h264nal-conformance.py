#!/usr/bin/env python3
"""Summarize a set of H264 bitstreams into a CSV file.

Runs h264nal on each input file and writes one CSV row per file, with
resolution, profile, level, NAL unit counts, and the parser status.

Example:
    $ ./h264nal-conformance.py -o summary.csv /path/to/dataset/*
"""

import argparse
import collections
import csv
import os
import re
import shutil
import subprocess
import sys


CSV_FIELDS = [
    "input_path",
    "input_file",
    "resolution",
    "profile",
    "level",
    "number_of_nalus",
    "number_of_slice_nalus",
    "number_of_slice_header_nalus",
    "h264nal_status",
    "slice_header_status",
]

STATUS_OK = "0"

# h264nal tells apart a bitstream it does not support from a broken one
EXIT_CODES = {
    1: "invalid bitstream",
    2: "unimplemented syntax",
}

DEFAULT_TIMEOUT = 60

# NAL unit types h264nal is expected to turn into a slice header: a coded
# slice (1), an IDR slice (5), and a coded slice extension (20). Data
# partitions (2 to 4) and auxiliary slices (19) are deliberately left out:
# h264nal does not parse them, so counting them would report a shortfall
# that is a missing feature rather than a failure.
SLICE_NAL_UNIT_TYPES = (1, 5, 20)

# a slice header is dumped as one of these, depending on whether the slice
# is a plain one or lives in an SVC scalable extension
SLICE_HEADER_MARKERS = ("slice_header {", "slice_header_in_scalable_extension {")

# Section A.3.1/A.3.2: level 1b is signalled differently depending on the
# profile. For the High profiles it is level_idc == 9; for the others it is
# level_idc == 11 together with constraint_set3_flag == 1.
HIGH_PROFILE_IDCS = {44, 100, 110, 122, 244}


def get_default_h264nal_path():
    """Find h264nal in $PATH, else fall back to the in-tree build."""
    path = shutil.which("h264nal")
    if path is not None:
        return path
    tools_dir = os.path.dirname(os.path.abspath(__file__))
    path = os.path.join(tools_dir, os.pardir, "build", "tools", "h264nal")
    path = os.path.normpath(path)
    return path if os.path.isfile(path) else None


def run_h264nal(input_path, h264nal_path, timeout):
    """Run h264nal on a file.

    Returns a (stdout, status) tuple. status is STATUS_OK when h264nal ran
    to completion without complaining, else a human-readable description.
    """
    try:
        result = subprocess.run(
            [
                h264nal_path,
                "-i",
                input_path,
                "--as-one-line",
                "--add-resolution",
            ],
            capture_output=True,
            text=True,
            timeout=timeout,
        )
    except subprocess.TimeoutExpired:
        return "", f"timeout after {timeout}s"
    except OSError as error:
        return "", f"could not run h264nal: {error}"

    status = summarize_errors(result.stderr)
    if result.returncode != 0:
        reason = EXIT_CODES.get(result.returncode)
        rc_status = (f"h264nal: {reason}" if reason is not None
                     else f"h264nal exited with {result.returncode}")
        status = rc_status if status == STATUS_OK else f"{rc_status}; {status}"
    return result.stdout, status


def summarize_errors(stderr):
    """Collapse h264nal's stderr into a single CSV-friendly status string.

    Parse errors repeat once per NAL unit (a rejected SPS yields one
    "non-existent SPS id" line per following slice), so identical messages
    are deduplicated and counted instead of being copied verbatim.
    """
    messages = [line.strip() for line in stderr.splitlines() if line.strip()]
    if not messages:
        return STATUS_OK
    counts = collections.Counter(messages)
    return "; ".join(
        message if count == 1 else f"{message} (x{count})"
        for message, count in counts.items()
    )


def get_int_field(text, name):
    match = re.search(rf"\b{name}: (-?\d+)", text)
    return int(match.group(1)) if match else None


def format_level(profile_idc, level_idc, constraint_set3_flag):
    """Convert level_idc into a level number (Sections A.3.1, A.3.2)."""
    if level_idc is None:
        return None
    if profile_idc in HIGH_PROFILE_IDCS:
        if level_idc == 9:
            return "1b"
    elif level_idc == 11 and constraint_set3_flag == 1:
        return "1b"
    return f"{level_idc // 10}.{level_idc % 10}"


def parse_sps_lines(stdout):
    """Extract (resolution, profile, level) from every SPS in the output.

    A bitstream may carry several SPSs (resolution changes, SVC subset
    SPSs), so each value is returned as an ordered list of unique entries.
    """
    resolutions = []
    profiles = []
    levels = []

    for line in stdout.splitlines():
        if "sps_data {" not in line:
            continue

        profile_idc = get_int_field(line, "profile_idc")
        level_idc = get_int_field(line, "level_idc")
        constraint_set3_flag = get_int_field(line, "constraint_set3_flag")
        width = get_int_field(line, "width")
        height = get_int_field(line, "height")

        profile_match = re.search(r"\bprofile: (.+?) level_idc:", line)
        profile = profile_match.group(1).strip() if profile_match else None

        if width is not None and height is not None:
            add_unique(resolutions, f"{width}x{height}")
        add_unique(profiles, profile)
        add_unique(levels, format_level(profile_idc, level_idc,
                                        constraint_set3_flag))

    return resolutions, profiles, levels


def add_unique(values, value):
    if value is not None and value not in values:
        values.append(value)


def count_slice_nalus(stdout):
    """Count the NAL units that should each yield one slice header."""
    types = "|".join(str(t) for t in SLICE_NAL_UNIT_TYPES)
    return len(re.findall(rf"\bnal_unit_type: (?:{types})\b", stdout))


def count_slice_headers(stdout):
    """Count the slice headers h264nal actually parsed."""
    return sum(stdout.count(marker) for marker in SLICE_HEADER_MARKERS)


def get_slice_header_status(slice_nalus, slice_headers):
    """Flag slice headers that went missing without h264nal saying so.

    A slice NAL unit that yields no slice header was rejected somewhere. The
    parser usually explains itself on stderr, but not always: a sub-parser
    can return nullptr silently and the NAL unit is then dumped with an empty
    payload. Comparing the two counts catches that, which stderr alone does
    not. Kept out of h264nal_status so that column stays a faithful record
    of what h264nal itself reported.
    """
    if slice_headers == slice_nalus:
        return STATUS_OK
    return f"unparsed slice headers: {slice_headers}/{slice_nalus}"


def analyze_file(input_path, h264nal_path, timeout):
    """Build the CSV row for a single input file."""
    stdout, status = run_h264nal(input_path, h264nal_path, timeout)
    resolutions, profiles, levels = parse_sps_lines(stdout)
    slice_nalus = count_slice_nalus(stdout)
    slice_headers = count_slice_headers(stdout)

    return {
        "input_path": input_path,
        "input_file": os.path.basename(input_path),
        "resolution": ";".join(resolutions),
        "profile": ";".join(profiles),
        "level": ";".join(levels),
        # in one-line mode there is exactly one "nal_unit {" per NAL unit
        "number_of_nalus": stdout.count("nal_unit {"),
        "number_of_slice_nalus": slice_nalus,
        "number_of_slice_header_nalus": slice_headers,
        "h264nal_status": status,
        "slice_header_status": get_slice_header_status(slice_nalus,
                                                       slice_headers),
    }


def get_options(argv):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "infiles",
        nargs="+",
        help="H264 files to parse",
    )
    parser.add_argument(
        "-o",
        "--output",
        dest="outfile",
        default=None,
        help="CSV output file [default: stdout]",
    )
    parser.add_argument(
        "--h264nal",
        dest="h264nal_path",
        default=get_default_h264nal_path(),
        help="path to the h264nal binary [default: %(default)s]",
    )
    parser.add_argument(
        "--timeout",
        type=int,
        default=DEFAULT_TIMEOUT,
        help="per-file timeout, in seconds [default: %(default)s]",
    )
    parser.add_argument(
        "-q",
        "--quiet",
        action="store_true",
        help="do not print per-file progress",
    )
    return parser.parse_args(argv[1:])


def main(argv):
    options = get_options(argv)

    if options.h264nal_path is None:
        print("error: cannot find h264nal, use --h264nal", file=sys.stderr)
        sys.exit(1)

    # a shell glob may hand us directories: parse regular files only
    infiles = [name for name in options.infiles if not os.path.isdir(name)]

    rows = []
    for index, input_path in enumerate(infiles, start=1):
        if not options.quiet:
            print(f"[{index}/{len(infiles)}] {input_path}", file=sys.stderr)
        rows.append(analyze_file(input_path, options.h264nal_path,
                                 options.timeout))

    if options.outfile is None:
        write_csv(sys.stdout, rows)
    else:
        with open(options.outfile, "w", newline="") as fout:
            write_csv(fout, rows)

    if not options.quiet:
        failed = sum(1 for row in rows
                     if row["h264nal_status"] != STATUS_OK)
        # files losing slice headers without h264nal reporting anything:
        # worth calling out separately, as they look clean otherwise
        silent = sum(1 for row in rows
                     if row["h264nal_status"] == STATUS_OK
                     and row["slice_header_status"] != STATUS_OK)
        print(f"parsed {len(rows)} file(s), {failed} with issues, "
              f"{silent} silently missing slice headers",
              file=sys.stderr)


def write_csv(fout, rows):
    writer = csv.DictWriter(fout, fieldnames=CSV_FIELDS)
    writer.writeheader()
    writer.writerows(rows)


if __name__ == "__main__":
    main(sys.argv)
