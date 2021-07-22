/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_dec_ref_pic_marking_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h264_common.h"
#include "rtc_base/arraysize.h"
#include "rtc_base/bit_buffer.h"

namespace h264nal {

class H264DecRefPicMarkingParserTest : public ::testing::Test {
 public:
  H264DecRefPicMarkingParserTest() {}
  ~H264DecRefPicMarkingParserTest() override {}
};

#ifdef NOTDEF
TEST_F(H264DecRefPicMarkingParserTest, TestSampleDecRefPicMarking1) {
  // dec_ref_pic_marking
  // fuzzer::conv: data
  const uint8_t buffer[] = {0x10, 0xc6};
  // fuzzer::conv: begin
  auto dec_ref_pic_marking =
      H264DecRefPicMarkingParser::ParseDecRefPicMarking(buffer, arraysize(buffer), 1, 0);
  // fuzzer::conv: end

  EXPECT_TRUE(dec_ref_pic_marking != nullptr);

  EXPECT_EQ(7, dec_ref_pic_marking->luma_log2_weight_denom);
  EXPECT_EQ(-1, dec_ref_pic_marking->delta_chroma_log2_weight_denom);
  EXPECT_THAT(dec_ref_pic_marking->luma_weight_l0_flag,
              ::testing::ElementsAreArray({0}));
  EXPECT_THAT(dec_ref_pic_marking->chroma_weight_l0_flag,
              ::testing::ElementsAreArray({0}));
}
#endif

}  // namespace h264nal
