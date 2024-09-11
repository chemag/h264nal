/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#pragma once

#include <stdio.h>

#include <memory>
#include <vector>

#include "rtc_common.h"

namespace h264nal {

// A class for parsing out a HRD parameters
// (`hrd_parameters()`, as defined in Section E.1.2 of the 2012
// standard) from an H264 NALU.
class H264HrdParametersParser {
  // Section E.2.2: "The value of cpb_cnt_minus1 shall be in the range of
  // 0 to 31, inclusive."
  const static uint32_t kCpbCntMinus1Min = 0;
  const static uint32_t kCpbCntMinus1Max = 31;

 public:
  // The parsed state of the HrdParameters.
  struct HrdParametersState {
    HrdParametersState() = default;
    ~HrdParametersState() = default;
    // disable copy ctor, move ctor, and copy&move assignments
    HrdParametersState(const HrdParametersState&) = delete;
    HrdParametersState(HrdParametersState&&) = delete;
    HrdParametersState& operator=(const HrdParametersState&) = delete;
    HrdParametersState& operator=(HrdParametersState&&) = delete;

#ifdef FDUMP_DEFINE
    void fdump(FILE* outfp, int indent_level) const;
#endif  // FDUMP_DEFINE

    // contents
    uint32_t cpb_cnt_minus1 = 0;
    uint32_t bit_rate_scale = 0;
    uint32_t cpb_size_scale = 0;
    std::vector<uint32_t> bit_rate_value_minus1;
    std::vector<uint32_t> cpb_size_value_minus1;
    std::vector<uint32_t> cbr_flag;
    uint32_t initial_cpb_removal_delay_length_minus1 = 0;
    uint32_t cpb_removal_delay_length_minus1 = 0;
    uint32_t dpb_output_delay_length_minus1 = 0;
    uint32_t time_offset_length = 0;
  };

  // Unpack RBSP and parse HrdParameters state from the supplied buffer.
  static std::unique_ptr<HrdParametersState> ParseHrdParameters(
      const uint8_t* data, size_t length) noexcept;
  static std::unique_ptr<HrdParametersState> ParseHrdParameters(
      BitBuffer* bit_buffer) noexcept;
};

}  // namespace h264nal
