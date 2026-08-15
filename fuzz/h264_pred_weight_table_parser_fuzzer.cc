/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

// This file was auto-generated using fuzz/converter.py from
// h264_pred_weight_table_parser_unittest.cc.
// Do not edit directly.

#include "h264_pred_weight_table_parser.h"
#include "h264_common.h"
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
  uint32_t chroma_array_type = 1;
  uint32_t slice_type = SliceType::P_ALL;
  uint32_t num_ref_idx_l0_active_minus1 = 0;
  uint32_t num_ref_idx_l1_active_minus1 = 0;
  auto pred_weight_table = H264PredWeightTableParser::ParsePredWeightTable(
      data, size, chroma_array_type, slice_type,
      num_ref_idx_l0_active_minus1, num_ref_idx_l1_active_minus1);
  }
  {
  uint32_t chroma_array_type = 1;
  uint32_t slice_type = SliceType::P_ALL;
  uint32_t num_ref_idx_l0_active_minus1 = 12;
  uint32_t num_ref_idx_l1_active_minus1 = 0;
  auto pred_weight_table = H264PredWeightTableParser::ParsePredWeightTable(
      data, size, chroma_array_type, slice_type,
      num_ref_idx_l0_active_minus1, num_ref_idx_l1_active_minus1);
  }
  return 0;
}
