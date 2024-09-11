/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

// This file was auto-generated using fuzz/converter.py from
// h264_nal_unit_parser_unittest.cc.
// Do not edit directly.

#include "h264_nal_unit_parser.h"
#include "h264_bitstream_parser_state.h"
#include "h264_common.h"
#include "rtc_common.h"


// libfuzzer infra to test the fuzz target
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  {
  h264nal::H264BitstreamParserState bitstream_parser_state;
  h264nal::ParsingOptions parsing_options;
  parsing_options.add_checksum = true;
  auto nal_unit = h264nal::H264NalUnitParser::ParseNalUnit(data, size,
                                                  &bitstream_parser_state,
                                                  parsing_options);
  }
  return 0;
}
