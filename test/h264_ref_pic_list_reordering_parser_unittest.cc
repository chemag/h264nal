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

#ifdef NOTDEF
TEST_F(H264RefPicListReorderingParserTest, TestSampleRefPicListReordering1) {
  // ref_pic_list_reordering
  // fuzzer::conv: data
  const uint8_t buffer[] = {0x10, 0xc6};
  // fuzzer::conv: begin
  auto ref_pic_list_reordering =
      H264RefPicListReorderingParser::ParseRefPicListReordering(buffer, arraysize(buffer), 1, 0);
  // fuzzer::conv: end

  EXPECT_TRUE(ref_pic_list_reordering != nullptr);

  EXPECT_EQ(7, ref_pic_list_reordering->luma_log2_weight_denom);
  EXPECT_EQ(-1, ref_pic_list_reordering->delta_chroma_log2_weight_denom);
  EXPECT_THAT(ref_pic_list_reordering->luma_weight_l0_flag,
              ::testing::ElementsAreArray({0}));
  EXPECT_THAT(ref_pic_list_reordering->chroma_weight_l0_flag,
              ::testing::ElementsAreArray({0}));
}
#endif

}  // namespace h264nal
