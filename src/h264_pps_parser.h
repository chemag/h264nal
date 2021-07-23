/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#pragma once

#include <stdio.h>

#include <memory>
#include <vector>

#include "rtc_base/bit_buffer.h"

namespace h264nal {

// A class for parsing out a video sequence parameter set (PPS) data from
// an H264 NALU.
class H264PpsParser {
 public:
  // The parsed state of the PPS. Only some select values are stored.
  // Add more as they are actually needed.
  struct PpsState {
    PpsState() = default;
    ~PpsState() = default;
    // disable copy ctor, move ctor, and copy&move assignments
    PpsState(const PpsState&) = delete;
    PpsState(PpsState&&) = delete;
    PpsState& operator=(const PpsState&) = delete;
    PpsState& operator=(PpsState&&) = delete;

#ifdef FDUMP_DEFINE
    void fdump(FILE* outfp, int indent_level) const;
#endif  // FDUMP_DEFINE

    uint32_t pic_parameter_set_id = 0;
    uint32_t seq_parameter_set_id = 0;
    uint32_t entropy_coding_mode_flag = 0;
    uint32_t pic_order_present_flag = 0;
    uint32_t num_slice_groups_minus1 = 0;
    uint32_t slice_group_map_type = 0;
    std::vector<uint32_t> run_length_minus1;
    std::vector<uint32_t> top_left;
    std::vector<uint32_t> bottom_right;
    uint32_t slice_group_change_direction_flag = 0;
    uint32_t slice_group_change_rate_minus1 = 0;
    uint32_t pic_size_in_map_units_minus1 = 0;
    std::vector<uint32_t> slice_group_id;
    uint32_t num_ref_idx_l0_active_minus1 = 0;
    uint32_t num_ref_idx_l1_active_minus1 = 0;
    uint32_t weighted_pred_flag = 0;
    uint32_t weighted_bipred_idc = 0;
    int32_t pic_init_qp_minus26 = 0;
    int32_t pic_init_qs_minus26 = 0;
    int32_t chroma_qp_index_offset = 0;
    uint32_t deblocking_filter_control_present_flag = 0;
    uint32_t constrained_intra_pred_flag = 0;
    uint32_t redundant_pic_cnt_present_flag = 0;

    // derived values
    uint32_t getSliceGroupIdLen() noexcept;
  };

  // Unpack RBSP and parse PPS state from the supplied buffer.
  static std::shared_ptr<PpsState> ParsePps(const uint8_t* data,
                                            size_t length) noexcept;
  static std::shared_ptr<PpsState> ParsePps(
      rtc::BitBuffer* bit_buffer) noexcept;
};

}  // namespace h264nal