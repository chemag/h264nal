/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_sps_extension_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h264_common.h"
#include "h264_sps_parser.h"
#include "rtc_common.h"

namespace h264nal {

class H264SpsExtensionParserTest : public ::testing::Test {
 public:
  H264SpsExtensionParserTest() {}
  ~H264SpsExtensionParserTest() override {}
};

TEST_F(H264SpsExtensionParserTest, TestSample01) {
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      // 0 1   3     0 ff00ff  00ff00 0
      // 1 010 00100 0 111100001111 000011110000 0
      // 1010 0010 0011 1100 0011 11 00 0011 1100 00 0
      0xa2, 0x3c, 0x3c, 0x3c, 0x00
  };
  // fuzzer::conv: begin
  auto sps_extension =
      H264SpsExtensionParser::ParseSpsExtension(buffer, arraysize(buffer));
  // fuzzer::conv: end

  EXPECT_TRUE(sps_extension != nullptr);

  EXPECT_EQ(0, sps_extension->seq_parameter_set_id);
  EXPECT_EQ(1, sps_extension->aux_format_idc);
  EXPECT_EQ(3, sps_extension->bit_depth_aux_minus8);
  EXPECT_EQ(0, sps_extension->alpha_incr_flag);
  EXPECT_EQ(0xf0f, sps_extension->alpha_opaque_value);
  EXPECT_EQ(0x0f0, sps_extension->alpha_transparent_value);
  EXPECT_EQ(0, sps_extension->additional_extension_flag);
}

}  // namespace h264nal
