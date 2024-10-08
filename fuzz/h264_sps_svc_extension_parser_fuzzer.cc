/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

// This file was auto-generated using fuzz/converter.py from
// h264_sps_svc_extension_parser_unittest.cc.
// Do not edit directly.

#include "h264_sps_svc_extension_parser.h"
#include "h264_common.h"
#include "rtc_common.h"


// libfuzzer infra to test the fuzz target
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  {
  uint32_t ChromaArrayType = 0;
  auto sps_svc_extension = h264nal::H264SpsSvcExtensionParser::ParseSpsSvcExtension(
      data, size, ChromaArrayType);
  }
  {
  uint32_t ChromaArrayType = 1;
  auto sps_svc_extension = h264nal::H264SpsSvcExtensionParser::ParseSpsSvcExtension(
      data, size, ChromaArrayType);
  }
  {
  uint32_t ChromaArrayType = 1;
  auto sps_svc_extension = h264nal::H264SpsSvcExtensionParser::ParseSpsSvcExtension(
      data, size, ChromaArrayType);
  }
  return 0;
}
