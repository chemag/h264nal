/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

// This file was auto-generated using fuzz/converter.py from
// h264_rtp_parser_unittest.cc.
// Do not edit directly.

#include "h264_rtp_parser.h"
#include "h264_common.h"
#include "rtc_common.h"


// libfuzzer infra to test the fuzz target
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  {
  h264nal::H264BitstreamParserState bitstream_parser_state;
  auto rtp = h264nal::H264RtpParser::ParseRtp(data, size,
                                     &bitstream_parser_state);
  }
  return 0;
}
