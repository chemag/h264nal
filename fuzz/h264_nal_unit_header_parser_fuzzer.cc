/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

// Manually created fuzzer for h264_nal_unit_header_parser.
// No corresponding unittest with fuzzer::conv markers exists.

#include "h264_nal_unit_header_parser.h"
#include "rtc_common.h"


// libfuzzer infra to test the fuzz target
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  {
  auto nal_unit_header =
      h264nal::H264NalUnitHeaderParser::ParseNalUnitHeader(data, size);
  }
  return 0;
}
