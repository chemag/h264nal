/*
 *  Copyright (c) Meta, Inc. and its affiliates.
 */

#pragma once

#include <stdio.h>

#include <memory>
#include <vector>

#include "h264_sps_parser.h"
#include "rtc_base/bit_buffer.h"

namespace h264nal {

// A class for parsing out an sequence parameter set (SPS) extension
// from an H264 NALU.
class H264SpsExtensionParser {
 public:
  // The parsed state of the SPS extension. Only some select values are stored.
  // Add more as they are actually needed.
  struct SpsExtensionState {
    SpsExtensionState() = default;
    ~SpsExtensionState() = default;
    // disable copy ctor, move ctor, and copy&move assignments
    SpsExtensionState(const SpsExtensionState&) = delete;
    SpsExtensionState(SpsExtensionState&&) = delete;
    SpsExtensionState& operator=(const SpsExtensionState&) = delete;
    SpsExtensionState& operator=(SpsExtensionState&&) = delete;

#ifdef FDUMP_DEFINE
    void fdump(FILE* outfp, int indent_level) const;
#endif  // FDUMP_DEFINE

    uint32_t seq_parameter_set_id = 0;
    uint32_t aux_format_idc = 0;
    uint32_t bit_depth_aux_minus8 = 0;
    uint32_t alpha_incr_flag = 0;
    uint32_t alpha_opaque_value = 0;
    uint32_t alpha_transparent_value = 0;
    uint32_t additional_extension_flag = 0;
  };

  // Unpack RBSP and parse SPSExtensio state from the supplied buffer.
  static std::shared_ptr<SpsExtensionState> ParseSpsExtension(
      const uint8_t* data, size_t length) noexcept;
  static std::shared_ptr<SpsExtensionState> ParseSpsExtension(
      rtc::BitBuffer* bit_buffer) noexcept;
};

}  // namespace h264nal
