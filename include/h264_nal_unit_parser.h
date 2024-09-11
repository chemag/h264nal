/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#pragma once

#include <stdio.h>

#include <memory>

#include "h264_bitstream_parser_state.h"
#include "h264_common.h"
#include "h264_nal_unit_header_parser.h"
#include "h264_nal_unit_payload_parser.h"
#include "rtc_common.h"

namespace h264nal {

// A class for parsing out an H264 NAL Unit.
class H264NalUnitParser {
 public:
  // The parsed state of the NAL Unit. Only some select values are stored.
  // Add more as they are actually needed.
  struct NalUnitState {
    NalUnitState() = default;
    ~NalUnitState() = default;
    // disable copy ctor, move ctor, and copy&move assignments
    NalUnitState(const NalUnitState&) = delete;
    NalUnitState(NalUnitState&&) = delete;
    NalUnitState& operator=(const NalUnitState&) = delete;
    NalUnitState& operator=(NalUnitState&&) = delete;

#ifdef FDUMP_DEFINE
    void fdump(FILE* outfp, int indent_level,
               ParsingOptions parsing_options) const;
#endif  // FDUMP_DEFINE

    // NAL Unit offset in the full blob
    size_t offset;
    // NAL Unit length
    size_t length;
    // NAL Unit parsed length
    size_t parsed_length;
    // NAL Unit checksum
    std::shared_ptr<NaluChecksum> checksum;

    std::unique_ptr<struct H264NalUnitHeaderParser::NalUnitHeaderState>
        nal_unit_header;
    std::unique_ptr<struct H264NalUnitPayloadParser::NalUnitPayloadState>
        nal_unit_payload;
  };

  // Parse NAL unit state from the supplied buffer.
  // Use this function to parse NALUs that have not been escaped
  // into an RBSP, e.g. with NALUs from an mp4 mdat box.
  static std::unique_ptr<NalUnitState> ParseNalUnitUnescaped(
      const uint8_t* data, size_t length,
      struct H264BitstreamParserState* bitstream_parser_state,
      ParsingOptions parsing_options) noexcept;
  // Unpack RBSP and parse NAL unit state from the supplied buffer.
  // Use this function to parse NALUs that have been escaped
  // to avoid the start code prefix (0x000001/0x00000001)
  static std::unique_ptr<NalUnitState> ParseNalUnit(
      const uint8_t* data, size_t length,
      struct H264BitstreamParserState* bitstream_parser_state,
      ParsingOptions parsing_options) noexcept;
  static std::unique_ptr<NalUnitState> ParseNalUnit(
      BitBuffer* bit_buffer,
      struct H264BitstreamParserState* bitstream_parser_state,
      ParsingOptions parsing_options) noexcept;
};

}  // namespace h264nal
