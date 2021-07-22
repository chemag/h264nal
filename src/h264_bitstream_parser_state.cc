/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_bitstream_parser_state.h"

#include <stdio.h>

#include <cstdint>
#include <memory>

#include "h264_common.h"
#include "h264_pps_parser.h"
#include "h264_sps_parser.h"

namespace {
typedef std::shared_ptr<struct h264nal::H264SpsParser::SpsState>
    SharedPtrSpsState;
typedef std::shared_ptr<struct h264nal::H264PpsParser::PpsState>
    SharedPtrPpsState;
}  // namespace

namespace h264nal {

// General note: this is based off the 2004 version of the H.264 standard.
// You can find it on this page:
// http://www.itu.int/rec/T-REC-H.264

std::shared_ptr<struct H264SpsParser::SpsState>
H264BitstreamParserState::GetSps(uint32_t sps_id) const {
  // check the SPS exists in the bitstream parser state
  auto it = sps.find(sps_id);
  if (it == sps.end()) {
    return SharedPtrSpsState(nullptr);
  }
  return SharedPtrSpsState(it->second);
}

std::shared_ptr<struct H264PpsParser::PpsState>
H264BitstreamParserState::GetPps(uint32_t pps_id) const {
  // check the PPS exists in the bitstream parser state
  auto it = pps.find(pps_id);
  if (it == pps.end()) {
    return SharedPtrPpsState(nullptr);
  }
  return SharedPtrPpsState(it->second);
}

}  // namespace h264nal
