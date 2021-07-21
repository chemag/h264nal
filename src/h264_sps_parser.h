/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#pragma once

#include <stdio.h>

#include <memory>
#include <vector>

#include "h264_vui_parameters_parser.h"
#include "rtc_base/bit_buffer.h"

namespace h264nal {

// A class for parsing out a video sequence parameter set (SPS) data from
// an H264 NALU.
class H264SpsParser {
 public:
  // The parsed state of the SPS. Only some select values are stored.
  // Add more as they are actually needed.
  struct SpsState {
    SpsState() = default;
    ~SpsState() = default;
    // disable copy ctor, move ctor, and copy&move assignments
    SpsState(const SpsState&) = delete;
    SpsState(SpsState&&) = delete;
    SpsState& operator=(const SpsState&) = delete;
    SpsState& operator=(SpsState&&) = delete;

#ifdef FDUMP_DEFINE
    void fdump(FILE* outfp, int indent_level) const;
#endif  // FDUMP_DEFINE

    uint32_t profile_idc = 0;
    uint32_t constraint_set0_flag = 0;
    uint32_t constraint_set1_flag = 0;
    uint32_t constraint_set2_flag = 0;
    uint32_t reserved_zero_5bits = 0;
    uint32_t level_idc = 0;
    uint32_t seq_parameter_set_id = 0;
    uint32_t log2_max_frame_num_minus4 = 0;
    uint32_t pic_order_cnt_type = 0;
    uint32_t log2_max_pic_order_cnt_lsb_minus4 = 0;
    uint32_t delta_pic_order_always_zero_flag = 0;
    int32_t offset_for_non_ref_pic = 0;
    int32_t offset_for_top_to_bottom_field = 0;
    uint32_t num_ref_frames_in_pic_order_cnt_cycle = 0;
    std::vector<int32_t> offset_for_ref_frame;
    uint32_t num_ref_frames = 0;
    uint32_t gaps_in_frame_num_value_allowed_flag = 0;
    uint32_t pic_width_in_mbs_minus1 = 0;
    uint32_t pic_height_in_map_units_minus1 = 0;
    uint32_t frame_mbs_only_flag = 0;
    uint32_t mb_adaptive_frame_field_flag = 0;
    uint32_t direct_8x8_inference_flag = 0;
    uint32_t frame_cropping_flag = 0;
    uint32_t frame_crop_left_offset = 0;
    uint32_t frame_crop_right_offset = 0;
    uint32_t frame_crop_top_offset = 0;
    uint32_t frame_crop_bottom_offset = 0;
    uint32_t vui_parameters_present_flag = 0;
    std::unique_ptr<struct H264VuiParametersParser::VuiParametersState>
        vui_parameters;
  };

  // Unpack RBSP and parse SPS state from the supplied buffer.
  static std::shared_ptr<SpsState> ParseSps(const uint8_t* data,
                                            size_t length) noexcept;
  static std::shared_ptr<SpsState> ParseSps(
      rtc::BitBuffer* bit_buffer) noexcept;
};

}  // namespace h264nal
