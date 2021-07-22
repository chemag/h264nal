/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_pred_weight_table_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h264_common.h"
#include "rtc_base/arraysize.h"
#include "rtc_base/bit_buffer.h"

namespace h264nal {

class H264PredWeightTableParserTest : public ::testing::Test {
 public:
  H264PredWeightTableParserTest() {}
  ~H264PredWeightTableParserTest() override {}
};

#ifdef NOTDEF
TEST_F(H264PredWeightTableParserTest, TestSamplePredWeightTable1) {
  // pred_weight_table
  // fuzzer::conv: data
  const uint8_t buffer[] = {0x10, 0xc6};
  // fuzzer::conv: begin
  auto pred_weight_table =
      H264PredWeightTableParser::ParsePredWeightTable(buffer, arraysize(buffer), 1, 0);
  // fuzzer::conv: end

  EXPECT_TRUE(pred_weight_table != nullptr);

  EXPECT_EQ(7, pred_weight_table->luma_log2_weight_denom);
  EXPECT_EQ(-1, pred_weight_table->delta_chroma_log2_weight_denom);
  EXPECT_THAT(pred_weight_table->luma_weight_l0_flag,
              ::testing::ElementsAreArray({0}));
  EXPECT_THAT(pred_weight_table->chroma_weight_l0_flag,
              ::testing::ElementsAreArray({0}));
}
#endif

}  // namespace h264nal
