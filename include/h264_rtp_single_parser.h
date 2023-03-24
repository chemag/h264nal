/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#pragma once

#include <stdio.h>

#include <memory>

#include "h264_bitstream_parser_state.h"
#include "h264_nal_unit_parser.h"
#include "rtc_base/bit_buffer.h"

namespace h264nal {

// A class for parsing out an RTP Single NAL Unit data.
class H264RtpSingleParser {
 public:
  // The parsed state of the RTP Single NAL Unit.
  struct RtpSingleState {
    RtpSingleState() = default;
    ~RtpSingleState() = default;
    // disable copy ctor, move ctor, and copy&move assignments
    RtpSingleState(const RtpSingleState&) = delete;
    RtpSingleState(RtpSingleState&&) = delete;
    RtpSingleState& operator=(const RtpSingleState&) = delete;
    RtpSingleState& operator=(RtpSingleState&&) = delete;

#ifdef FDUMP_DEFINE
    void fdump(FILE* outfp, int indent_level) const;
#endif  // FDUMP_DEFINE

    std::unique_ptr<struct H264NalUnitHeaderParser::NalUnitHeaderState>
        nal_unit_header;
    std::unique_ptr<struct H264NalUnitPayloadParser::NalUnitPayloadState>
        nal_unit_payload;
  };

  // Unpack RBSP and parse RTP Single NAL Unit state from the supplied buffer.
  static std::unique_ptr<RtpSingleState> ParseRtpSingle(
      const uint8_t* data, size_t length,
      struct H264BitstreamParserState* bitstream_parser_state) noexcept;
  static std::unique_ptr<RtpSingleState> ParseRtpSingle(
      rtc::BitBuffer* bit_buffer,
      struct H264BitstreamParserState* bitstream_parser_state) noexcept;
};

}  // namespace h264nal
