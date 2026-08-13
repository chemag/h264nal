/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

// This file was auto-generated using fuzz/converter.py from
// h264_dec_ref_pic_marking_parser_unittest.cc.
// Do not edit directly.

#include "h264_dec_ref_pic_marking_parser.h"
#include "h264_common.h"
#include "rtc_common.h"


// libfuzzer infra to test the fuzz target
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  {
  uint32_t IdrPicFlag = 1;
  auto dec_ref_pic_marking = h264nal::H264DecRefPicMarkingParser::ParseDecRefPicMarking(
      data, size, IdrPicFlag);
  }
  {
  uint32_t IdrPicFlag = 0;
  auto dec_ref_pic_marking = h264nal::H264DecRefPicMarkingParser::ParseDecRefPicMarking(
      data, size, IdrPicFlag);
  }
  return 0;
}
