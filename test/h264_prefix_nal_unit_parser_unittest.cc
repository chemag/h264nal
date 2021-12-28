/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_prefix_nal_unit_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h264_common.h"
#include "rtc_base/arraysize.h"
#include "rtc_base/bit_buffer.h"

namespace h264nal {

class H264PrefixNalUnitRbspParserTest : public ::testing::Test {
 public:
  H264PrefixNalUnitRbspParserTest() {}
  ~H264PrefixNalUnitRbspParserTest() override {}
};

TEST_F(H264PrefixNalUnitRbspParserTest, TestPrefixNalUnitParser01) {
  // foreman.svc.264
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      0x20
  };
  // fuzzer::conv: begin
  uint32_t svc_extension_flag = 1;
  uint32_t nal_ref_idc = 2;
  uint32_t use_ref_base_pic_flag = 1;
  uint32_t idr_flag = 1;
  auto prefix_nal_unit_rbsp =
      H264PrefixNalUnitRbspParser::ParsePrefixNalUnitRbsp(
          buffer, arraysize(buffer), svc_extension_flag, nal_ref_idc,
          use_ref_base_pic_flag, idr_flag);
  // fuzzer::conv: end

  EXPECT_TRUE(prefix_nal_unit_rbsp != nullptr);

  // prefix_nal_unit_svc()
  auto& prefix_nal_unit_svc = prefix_nal_unit_rbsp->prefix_nal_unit_svc;
  EXPECT_TRUE(prefix_nal_unit_svc != nullptr);

  EXPECT_EQ(0, prefix_nal_unit_svc->store_ref_base_pic_flag);
  EXPECT_EQ(0, prefix_nal_unit_svc->additional_prefix_nal_unit_extension_flag);
  EXPECT_EQ(
      0, prefix_nal_unit_svc->additional_prefix_nal_unit_extension_data_flag);
}

}  // namespace h264nal
