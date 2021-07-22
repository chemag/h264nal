/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#pragma once

#include <stdio.h>

#include <memory>
#include <vector>

#include "rtc_base/bit_buffer.h"

namespace h264nal {

// A class for parsing out a reference picture list reordering
// (`ref_pic_list_reordering()`, as defined in Section 7.3.3.1 of the 2004
// standard) from an H264 NALU.
class H264RefPicListReorderingParser {
 public:
  // The parsed state of the RefPicListReordering.
  struct RefPicListReorderingState {
    RefPicListReorderingState() = default;
    ~RefPicListReorderingState() = default;
    // disable copy ctor, move ctor, and copy&move assignments
    RefPicListReorderingState(const RefPicListReorderingState&) = delete;
    RefPicListReorderingState(RefPicListReorderingState&&) = delete;
    RefPicListReorderingState& operator=(const RefPicListReorderingState&) =
        delete;
    RefPicListReorderingState& operator=(RefPicListReorderingState&&) = delete;

#ifdef FDUMP_DEFINE
    void fdump(FILE* outfp, int indent_level) const;
#endif  // FDUMP_DEFINE

    // input parameters
    uint32_t slice_type = 0;

    // contents
    uint32_t ref_pic_list_reordering_flag_l0 = 0;
    std::vector<uint32_t> reordering_of_pic_nums_idc;
    std::vector<uint32_t> abs_diff_pic_num_minus1;
    std::vector<uint32_t> long_term_pic_num;
    uint32_t ref_pic_list_reordering_flag_l1 = 0;
  };

  // Unpack RBSP and parse RefPicListReordering state from the supplied buffer.
  static std::unique_ptr<RefPicListReorderingState> ParseRefPicListReordering(
      const uint8_t* data, size_t length, uint32_t slice_type) noexcept;
  static std::unique_ptr<RefPicListReorderingState> ParseRefPicListReordering(
      rtc::BitBuffer* bit_buffer, uint32_t slice_type) noexcept;
};

}  // namespace h264nal
