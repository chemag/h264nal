/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_ref_pic_list_reordering_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h264_common.h"
#include "rtc_base/arraysize.h"
#include "rtc_base/bit_buffer.h"

namespace h264nal {

class H264RefPicListReorderingParserTest : public ::testing::Test {
 public:
  H264RefPicListReorderingParserTest() {}
  ~H264RefPicListReorderingParserTest() override {}
};

TEST_F(H264RefPicListReorderingParserTest,
       TestSampleRefPicListReordering601NonIDR) {
  // ref_pic_list_reordering
  // fuzzer::conv: data
  const uint8_t buffer[] = {0x00};
  // fuzzer::conv: begin
  uint32_t slice_type = SliceType::P_ALL;
  auto ref_pic_list_reordering =
      H264RefPicListReorderingParser::ParseRefPicListReordering(
          buffer, arraysize(buffer), slice_type);
  // fuzzer::conv: end

  EXPECT_TRUE(ref_pic_list_reordering != nullptr);

  EXPECT_EQ(0, ref_pic_list_reordering->ref_pic_list_reordering_flag_l0);
}

}  // namespace h264nal
