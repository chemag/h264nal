#!/usr/bin/env python3
"""Parse H.264 Annex B bitstream via h264nal and simulate DPB management."""

import argparse
import os
import re
import subprocess
import sys
import tempfile


import shutil


DEFAULT_H264NAL_PATH = shutil.which("h264nal")


def run_h264nal(input_path, h264nal_path):
    result = subprocess.run(
        [h264nal_path, "-i", input_path, "--no-as-one-line"],
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        print(f"h264nal failed: {result.stderr}", file=sys.stderr)
        sys.exit(1)
    return result.stdout


def parse_frames(content):
    nal_units = re.split(r"^nal_unit \{", content, flags=re.MULTILINE)
    sps = {}
    pps = {}
    frames = []

    for nal_text in nal_units[1:]:
        nal_type_m = re.search(r"nal_unit_type: (\d+)", nal_text)
        if not nal_type_m:
            continue
        nal_type = int(nal_type_m.group(1))

        if nal_type == 7:
            sps["log2_max_frame_num_minus4"] = int(
                re.search(r"log2_max_frame_num_minus4: (\d+)", nal_text).group(1)
            )
            sps["max_num_ref_frames"] = int(
                re.search(r"max_num_ref_frames: (\d+)", nal_text).group(1)
            )
            sps["pic_order_cnt_type"] = int(
                re.search(r"pic_order_cnt_type: (\d+)", nal_text).group(1)
            )
            continue

        if nal_type == 8:
            pps["num_ref_idx_l0_default_active_minus1"] = int(
                re.search(
                    r"num_ref_idx_l0_default_active_minus1: (\d+)", nal_text
                ).group(1)
            )
            continue

        if nal_type not in (1, 5):
            continue

        first_mb_m = re.search(r"first_mb_in_slice: (\d+)", nal_text)
        if not first_mb_m or int(first_mb_m.group(1)) != 0:
            continue

        slice_type = int(re.search(r"slice_type: (\d+)", nal_text).group(1))
        frame_num = int(re.search(r"frame_num: (\d+)", nal_text).group(1))
        nal_ref_idc = int(re.search(r"nal_ref_idc: (\d+)", nal_text).group(1))

        frame = {
            "nal_type": nal_type,
            "slice_type": slice_type,
            "frame_num": frame_num,
            "nal_ref_idc": nal_ref_idc,
            "is_idr": nal_type == 5,
            "is_ref": nal_ref_idc > 0,
        }

        override_m = re.search(r"num_ref_idx_active_override_flag: (\d+)", nal_text)
        if override_m and int(override_m.group(1)) == 1:
            l0_m = re.search(r"num_ref_idx_l0_active_minus1: (\d+)", nal_text)
            frame["num_ref_idx_l0_active"] = int(l0_m.group(1)) + 1
        else:
            frame["num_ref_idx_l0_active"] = (
                pps.get("num_ref_idx_l0_default_active_minus1", 0) + 1
            )

        if nal_type == 5:
            ltr_m = re.search(r"long_term_reference_flag: (\d+)", nal_text)
            frame["long_term_reference_flag"] = int(ltr_m.group(1)) if ltr_m else 0

        adaptive_m = re.search(r"adaptive_ref_pic_marking_mode_flag: (\d+)", nal_text)
        if adaptive_m and int(adaptive_m.group(1)) == 1:
            mmco_m = re.search(
                r"memory_management_control_operation \{ ([^}]*)\}", nal_text
            )
            diff_m = re.search(r"difference_of_pic_nums_minus1 \{ ([^}]*)\}", nal_text)
            ltfi_m = re.search(r"long_term_frame_idx \{ ([^}]*)\}", nal_text)
            mltfi_m = re.search(r"max_long_term_frame_idx_plus1 \{ ([^}]*)\}", nal_text)
            frame["mmco"] = [int(x) for x in mmco_m.group(1).split()] if mmco_m else []
            frame["diff_pic_nums"] = (
                [int(x) for x in diff_m.group(1).split()]
                if diff_m and diff_m.group(1).strip()
                else []
            )
            frame["long_term_frame_idx"] = (
                [int(x) for x in ltfi_m.group(1).split()]
                if ltfi_m and ltfi_m.group(1).strip()
                else []
            )
            frame["max_long_term_frame_idx_plus1"] = (
                [int(x) for x in mltfi_m.group(1).split()]
                if mltfi_m and mltfi_m.group(1).strip()
                else []
            )
        else:
            frame["mmco"] = []

        rplm_l0_m = re.search(r"ref_pic_list_modification_flag_l0: (\d+)", nal_text)
        frame["rplm_l0"] = int(rplm_l0_m.group(1)) if rplm_l0_m else 0
        if frame["rplm_l0"]:
            mod_idc_m = re.search(
                r"modification_of_pic_nums_idc \{ ([^}]*)\}", nal_text
            )
            abs_diff_m = re.search(r"abs_diff_pic_num_minus1 \{ ([^}]*)\}", nal_text)
            lt_pic_num_m = re.search(r"long_term_pic_num \{ ([^}]*)\}", nal_text)
            frame["rplm_mod_idc"] = (
                [int(x) for x in mod_idc_m.group(1).split()] if mod_idc_m else []
            )
            frame["rplm_abs_diff"] = (
                [int(x) for x in abs_diff_m.group(1).split()]
                if abs_diff_m and abs_diff_m.group(1).strip()
                else []
            )
            frame["rplm_lt_pic_num"] = (
                [int(x) for x in lt_pic_num_m.group(1).split()]
                if lt_pic_num_m and lt_pic_num_m.group(1).strip()
                else []
            )

        frames.append(frame)

    return sps, frames


def slice_type_str(st):
    return {0: "P", 1: "B", 2: "I", 3: "SP", 4: "SI"}.get(st, f"?{st}")


def frame_type_str(frame):
    if frame["is_idr"]:
        return "IDR"
    return slice_type_str(frame["slice_type"])


def entry_str(entry):
    tag = f"fn={entry['frame_num']}"
    if entry["is_ltr"]:
        tag += f"/LTR[{entry['ltr_idx']}]"
    else:
        tag += "/STR"
    return tag


def dpb_str(dpb):
    return ";".join(entry_str(e) for e in dpb)


def build_ref_pic_list0(dpb, frame):
    strs = sorted(
        [e for e in dpb if not e["is_ltr"]],
        key=lambda e: e["frame_num"],
        reverse=True,
    )
    ltrs = sorted(
        [e for e in dpb if e["is_ltr"]],
        key=lambda e: e["ltr_idx"],
    )
    return strs + ltrs


def apply_rplm(ref_list, frame, max_frame_num):
    if not frame.get("rplm_l0"):
        return ref_list

    ref_list = list(ref_list)
    mod_idcs = frame["rplm_mod_idc"]
    abs_diffs = list(frame.get("rplm_abs_diff", []))
    lt_pic_nums = list(frame.get("rplm_lt_pic_num", []))

    pic_num_pred = frame["frame_num"]
    ref_idx = 0
    abs_diff_idx = 0
    lt_idx = 0

    for idc in mod_idcs:
        if idc == 3:
            break
        if idc in (0, 1):
            abs_diff = abs_diffs[abs_diff_idx]
            abs_diff_idx += 1
            if idc == 0:
                pic_num_pred = (pic_num_pred - (abs_diff + 1)) % max_frame_num
            else:
                pic_num_pred = (pic_num_pred + (abs_diff + 1)) % max_frame_num
            target_fn = pic_num_pred
            found = None
            for i, e in enumerate(ref_list):
                if not e["is_ltr"] and e["frame_num"] == target_fn:
                    found = i
                    break
            if found is not None:
                entry = ref_list.pop(found)
                ref_list.insert(ref_idx, entry)
                ref_idx += 1
        elif idc == 2:
            lt_num = lt_pic_nums[lt_idx]
            lt_idx += 1
            found = None
            for i, e in enumerate(ref_list):
                if e["is_ltr"] and e["ltr_idx"] == lt_num:
                    found = i
                    break
            if found is not None:
                entry = ref_list.pop(found)
                ref_list.insert(ref_idx, entry)
                ref_idx += 1

    return ref_list


def refs_used_str(ref_list, num_active):
    active = ref_list[:num_active]
    return ";".join(entry_str(e) for e in active)


def mmco_str(frame):
    if frame.get("is_idr"):
        if frame.get("long_term_reference_flag", 0):
            return "IDR:LTR[0]"
        return "IDR:STR"
    ops = frame.get("mmco", [])
    if not ops:
        if frame.get("is_ref"):
            return "sliding_window"
        return ""
    parts = []
    diff_idx = 0
    ltfi_idx = 0
    mltfi_idx = 0
    for op in ops:
        if op == 0:
            break
        elif op == 1:
            diff = frame["diff_pic_nums"][diff_idx]
            diff_idx += 1
            pic_num = frame["frame_num"] - (diff + 1)
            parts.append(f"MMCO1:rm_STR(fn={pic_num})")
        elif op == 2:
            parts.append("MMCO2:rm_LTR")
        elif op == 3:
            diff = frame["diff_pic_nums"][diff_idx]
            diff_idx += 1
            pic_num = frame["frame_num"] - (diff + 1)
            ltf = frame["long_term_frame_idx"][ltfi_idx]
            ltfi_idx += 1
            parts.append(f"MMCO3:STR(fn={pic_num})->LTR[{ltf}]")
        elif op == 4:
            mlt = frame["max_long_term_frame_idx_plus1"][mltfi_idx]
            mltfi_idx += 1
            parts.append(f"MMCO4:max_ltr_idx={mlt - 1}")
        elif op == 5:
            parts.append("MMCO5:clear_all")
        elif op == 6:
            ltf = frame["long_term_frame_idx"][ltfi_idx]
            ltfi_idx += 1
            parts.append(f"MMCO6:self->LTR[{ltf}]")
    return ";".join(parts)


def simulate_dpb(sps, frames, outfile):
    max_frame_num = 1 << (sps["log2_max_frame_num_minus4"] + 4)
    max_num_ref_frames = sps["max_num_ref_frames"]

    dpb = []
    gop_num = 0
    gop_frame = 0

    outfile.write(
        "abs_frame,gop,gop_frame,type,frame_num,nal_ref_idc,ref,"
        "refs_used,dpb_before_decode,mmco,dpb_after_mmco\n"
    )

    for abs_frame, frame in enumerate(frames):
        if frame["is_idr"]:
            dpb = []
            gop_num += 1 if abs_frame > 0 else 0
            gop_frame = 0

        dpb_before = dpb_str(dpb)

        if frame["slice_type"] in (0, 1) and dpb:
            ref_list = build_ref_pic_list0(dpb, frame)
            ref_list = apply_rplm(ref_list, frame, max_frame_num)
            num_active = frame.get("num_ref_idx_l0_active", 1)
            refs = refs_used_str(ref_list, num_active)
        else:
            refs = ""

        ft = frame_type_str(frame)
        ref_mark = "Y" if frame["is_ref"] else "N"
        mmco = mmco_str(frame)

        if frame["is_idr"]:
            if frame.get("long_term_reference_flag", 0):
                dpb.append(
                    {
                        "frame_num": frame["frame_num"],
                        "is_ltr": True,
                        "ltr_idx": 0,
                        "abs_frame": abs_frame,
                    }
                )
            elif frame["is_ref"]:
                dpb.append(
                    {
                        "frame_num": frame["frame_num"],
                        "is_ltr": False,
                        "ltr_idx": -1,
                        "abs_frame": abs_frame,
                    }
                )
        elif frame["mmco"]:
            diff_idx = 0
            ltfi_idx = 0
            mltfi_idx = 0
            ops = frame["mmco"]
            i = 0
            while i < len(ops):
                op = ops[i]
                if op == 0:
                    break
                elif op == 1:
                    diff = frame["diff_pic_nums"][diff_idx]
                    diff_idx += 1
                    pic_num_to_remove = frame["frame_num"] - (diff + 1)
                    dpb = [
                        e
                        for e in dpb
                        if not (not e["is_ltr"] and e["frame_num"] == pic_num_to_remove)
                    ]
                elif op == 2:
                    pass
                elif op == 3:
                    diff = frame["diff_pic_nums"][diff_idx]
                    diff_idx += 1
                    pic_num = frame["frame_num"] - (diff + 1)
                    ltf_idx = frame["long_term_frame_idx"][ltfi_idx]
                    ltfi_idx += 1
                    for e in dpb:
                        if not e["is_ltr"] and e["frame_num"] == pic_num:
                            e["is_ltr"] = True
                            e["ltr_idx"] = ltf_idx
                            break
                elif op == 4:
                    max_lt = frame["max_long_term_frame_idx_plus1"][mltfi_idx]
                    mltfi_idx += 1
                    if max_lt == 0:
                        dpb = [e for e in dpb if not e["is_ltr"]]
                    else:
                        dpb = [
                            e
                            for e in dpb
                            if not (e["is_ltr"] and e["ltr_idx"] >= max_lt)
                        ]
                elif op == 5:
                    dpb = []
                elif op == 6:
                    ltf_idx = frame["long_term_frame_idx"][ltfi_idx]
                    ltfi_idx += 1
                    dpb = [
                        e for e in dpb if not (e["is_ltr"] and e["ltr_idx"] == ltf_idx)
                    ]
                    dpb.append(
                        {
                            "frame_num": frame["frame_num"],
                            "is_ltr": True,
                            "ltr_idx": ltf_idx,
                            "abs_frame": abs_frame,
                        }
                    )
                i += 1
        elif frame["is_ref"]:
            dpb.append(
                {
                    "frame_num": frame["frame_num"],
                    "is_ltr": False,
                    "ltr_idx": -1,
                    "abs_frame": abs_frame,
                }
            )
            str_count = sum(1 for e in dpb if not e["is_ltr"])
            ltr_count = sum(1 for e in dpb if e["is_ltr"])
            while str_count + ltr_count > max_num_ref_frames:
                for j, e in enumerate(dpb):
                    if not e["is_ltr"]:
                        dpb.pop(j)
                        str_count -= 1
                        break

        dpb_after = dpb_str(dpb)

        outfile.write(
            f"{abs_frame},{gop_num},{gop_frame},{ft},"
            f"{frame['frame_num']},{frame['nal_ref_idc']},{ref_mark},"
            f"{refs},{dpb_before},{mmco},{dpb_after}\n"
        )

        gop_frame += 1


def parse_args():
    parser = argparse.ArgumentParser(
        description="Parse H.264 Annex B bitstream and simulate DPB management."
    )
    parser.add_argument(
        "-i",
        "--input",
        required=True,
        help="Input H.264 Annex B bitstream file (.264/.h264).",
    )
    parser.add_argument(
        "-o",
        "--output",
        default=None,
        help="Output CSV file. Defaults to stdout.",
    )
    parser.add_argument(
        "--h264nal-path",
        default=DEFAULT_H264NAL_PATH,
        help="Path to h264nal binary (default: result of `which h264nal`).",
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

    print(f"Running h264nal on {args.input} ...", file=sys.stderr)
    h264nal_output = run_h264nal(args.input, args.h264nal_path)

    sps, frames = parse_frames(h264nal_output)
    print(
        f"SPS: MaxFrameNum={1 << (sps['log2_max_frame_num_minus4']+4)}, "
        f"max_num_ref_frames={sps['max_num_ref_frames']}, "
        f"pic_order_cnt_type={sps['pic_order_cnt_type']}",
        file=sys.stderr,
    )
    print(f"Total pictures: {len(frames)}", file=sys.stderr)

    if args.output:
        with open(args.output, "w") as f:
            simulate_dpb(sps, frames, f)
        print(f"Wrote {args.output}", file=sys.stderr)
    else:
        simulate_dpb(sps, frames, sys.stdout)


if __name__ == "__main__":
    main()
