#!/usr/bin/env python3
"""Split H.264 Annex B bitstream at resolution changes.

Runs h264nal to parse the input file, detects SPS NAL units where the
resolution changes, and splits the binary file at those boundaries.

Example:
    $ python3 h264nal-split.py -i foo.264
    # produces foo.01.264, foo.02.264, ...
"""

import argparse
import os
import re
import shutil
import subprocess
import sys


DEFAULT_H264NAL_PATH = shutil.which("h264nal")


def run_h264nal(input_path, h264nal_path):
    result = subprocess.run(
        [
            h264nal_path,
            "-i",
            input_path,
            "--no-as-one-line",
            "--add-offset",
            "--add-length",
            "--add-resolution",
        ],
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        print(f"h264nal failed: {result.stderr}", file=sys.stderr)
        sys.exit(1)
    return result.stdout


def parse_nal_units(content):
    """Parse h264nal multi-line output into a list of NAL unit dicts.

    Each dict has: offset, length, nal_unit_type.
    SPS NAL units (type 7) additionally have: width, height.
    """
    nal_texts = re.split(r"^nal_unit \{", content, flags=re.MULTILINE)
    nal_units = []

    for nal_text in nal_texts[1:]:
        nal = {}

        offset_m = re.search(r"offset: 0x([0-9a-f]+)", nal_text)
        if offset_m:
            nal["offset"] = int(offset_m.group(1), 16)

        length_m = re.search(r"length: (\d+)", nal_text)
        if length_m:
            nal["length"] = int(length_m.group(1))

        type_m = re.search(r"nal_unit_type: (\d+)", nal_text)
        if type_m:
            nal["nal_unit_type"] = int(type_m.group(1))

        width_m = re.search(r"width: (\d+)", nal_text)
        height_m = re.search(r"height: (\d+)", nal_text)
        if width_m and height_m:
            nal["width"] = int(width_m.group(1))
            nal["height"] = int(height_m.group(1))

        nal_units.append(nal)

    return nal_units


def find_start_code_offset(data, payload_offset):
    """Find the byte position where the Annex B start code begins.

    payload_offset points to the first byte after the start code (the NAL
    unit header).  The start code is either 00 00 01 (3 bytes) or
    00 00 00 01 (4 bytes).
    """
    if (
        payload_offset >= 4
        and data[payload_offset - 4 : payload_offset] == b"\x00\x00\x00\x01"
    ):
        return payload_offset - 4
    if (
        payload_offset >= 3
        and data[payload_offset - 3 : payload_offset] == b"\x00\x00\x01"
    ):
        return payload_offset - 3
    return payload_offset


def find_split_points(nal_units, data, debug=0):
    """Return byte offsets where the file should be split.

    A split happens before every SPS NAL unit whose resolution differs
    from the previously seen SPS.
    """
    split_points = []
    current_resolution = None

    for nal in nal_units:
        if nal.get("nal_unit_type") != 7:
            continue
        if "width" not in nal or "height" not in nal:
            continue

        resolution = (nal["width"], nal["height"])
        if current_resolution is not None and resolution != current_resolution:
            sc_offset = find_start_code_offset(data, nal["offset"])
            split_points.append(sc_offset)
            if debug > 0:
                print(
                    f"  resolution change at offset 0x{sc_offset:08x}: "
                    f"{current_resolution[0]}x{current_resolution[1]} -> "
                    f"{resolution[0]}x{resolution[1]}",
                    file=sys.stderr,
                )
        elif current_resolution is None and debug > 0:
            print(
                f"  initial resolution: {resolution[0]}x{resolution[1]}",
                file=sys.stderr,
            )
        current_resolution = resolution

    return split_points


def split_file(input_path, data, split_points, debug=0):
    """Write segments to foo.01.ext, foo.02.ext, ..."""
    base, ext = os.path.splitext(input_path)
    boundaries = [0] + split_points + [len(data)]
    output_files = []

    for i in range(len(boundaries) - 1):
        start = boundaries[i]
        end = boundaries[i + 1]
        filename = f"{base}.{i + 1:02d}{ext}"
        with open(filename, "wb") as f:
            f.write(data[start:end])
        output_files.append((filename, end - start))
        if debug > 0:
            print(
                f"  {filename}: bytes [0x{start:08x}, 0x{end:08x}) "
                f"({end - start} bytes)",
                file=sys.stderr,
            )

    return output_files


def parse_args():
    parser = argparse.ArgumentParser(
        description="Split H.264 Annex B bitstream at resolution changes."
    )
    parser.add_argument(
        "-i",
        "--input",
        required=True,
        help="Input H.264 Annex B bitstream file (.264/.h264).",
    )
    parser.add_argument(
        "--h264nal-path",
        default=DEFAULT_H264NAL_PATH,
        help="Path to h264nal binary (default: result of `which h264nal`).",
    )
    parser.add_argument(
        "-d",
        "--debug",
        action="count",
        default=0,
        help="Increase debug verbosity.",
    )
    return parser.parse_args()


def main():
    args = parse_args()

    if not os.path.isfile(args.input):
        print(f"error: input file not found: {args.input}", file=sys.stderr)
        sys.exit(1)

    if not args.h264nal_path or not os.path.isfile(args.h264nal_path):
        print(
            "error: h264nal binary not found. Install it or use --h264nal-path.",
            file=sys.stderr,
        )
        sys.exit(1)

    # 1. run h264nal to get NAL unit info
    print(f"Running h264nal on {args.input} ...", file=sys.stderr)
    h264nal_output = run_h264nal(args.input, args.h264nal_path)

    # 2. parse the output
    nal_units = parse_nal_units(h264nal_output)
    sps_count = sum(1 for n in nal_units if n.get("nal_unit_type") == 7)
    print(
        f"Found {len(nal_units)} NAL units ({sps_count} SPS)",
        file=sys.stderr,
    )

    # 3. read the raw binary
    with open(args.input, "rb") as f:
        data = f.read()

    # 4. find resolution-change split points
    split_points = find_split_points(nal_units, data, args.debug)

    if not split_points:
        print(
            "No resolution changes detected. No splitting needed.",
            file=sys.stderr,
        )
        return

    print(f"Found {len(split_points)} resolution change(s)", file=sys.stderr)

    # 5. write output segments
    output_files = split_file(args.input, data, split_points, args.debug)
    print(f"Created {len(output_files)} output file(s):", file=sys.stderr)
    for filename, size in output_files:
        print(f"  {filename} ({size} bytes)", file=sys.stderr)


if __name__ == "__main__":
    main()
