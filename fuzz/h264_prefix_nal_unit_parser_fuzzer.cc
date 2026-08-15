/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

// This file was auto-generated using fuzz/converter.py from
// h264_prefix_nal_unit_parser_unittest.cc.
// Do not edit directly.

#include "h264_prefix_nal_unit_parser.h"
#include "h264_common.h"
#include "h264_nal_unit_parser.h"
#include "rtc_common.h"


// libfuzzer infra to test the fuzz target
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  // the code below is copied verbatim out of a unit test, where it sits
  // inside "namespace h264nal", and so refers to the project's types
  // unqualified. This entry point cannot: it has to be extern "C" at
  // global scope. Pull the namespace in rather than qualifying each name,
  // which would mean keeping a list of them here.
  using namespace h264nal;  // NOLINT(build/namespaces)
  // a test with no fuzzer::conv markers converts to an empty body
  (void)data;
  (void)size;
  {
  uint32_t svc_extension_flag = 1;
  uint32_t nal_ref_idc = 2;
  uint32_t use_ref_base_pic_flag = 1;
  uint32_t idr_flag = 1;
  auto prefix_nal_unit_rbsp =
      H264PrefixNalUnitRbspParser::ParsePrefixNalUnitRbsp(
          data, size, svc_extension_flag, nal_ref_idc,
          use_ref_base_pic_flag, idr_flag);
  }
  {
  H264BitstreamParserState bitstream_parser_state;
  ParsingOptions parsing_options;
  parsing_options.add_checksum = true;
  auto nal_unit = H264NalUnitParser::ParseNalUnit(data, size,
                                                  &bitstream_parser_state,
                                                  parsing_options);
  }
  {
  H264BitstreamParserState bitstream_parser_state;
  ParsingOptions parsing_options;
  parsing_options.add_checksum = true;
  auto nal_unit = H264NalUnitParser::ParseNalUnit(data, size,
                                                  &bitstream_parser_state,
                                                  parsing_options);
  }
  return 0;
}
