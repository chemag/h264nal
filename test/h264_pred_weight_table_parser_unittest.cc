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

TEST_F(H264PredWeightTableParserTest, TestSamplePredWeightTable1) {
  // pred_weight_table
  // fuzzer::conv: data
  const uint8_t buffer[] = {0x6c};
  // fuzzer::conv: begin
  uint32_t slice_type = SliceType::P_ALL;
  uint32_t num_ref_idx_l0_active_minus1 = 0;
  uint32_t num_ref_idx_l1_active_minus1 = 0;

  auto pred_weight_table = H264PredWeightTableParser::ParsePredWeightTable(
      buffer, arraysize(buffer), slice_type, num_ref_idx_l0_active_minus1,
      num_ref_idx_l1_active_minus1);
  // fuzzer::conv: end

  EXPECT_TRUE(pred_weight_table != nullptr);

  EXPECT_EQ(2, pred_weight_table->luma_log2_weight_denom);
  EXPECT_EQ(2, pred_weight_table->chroma_log2_weight_denom);
  EXPECT_EQ(1, pred_weight_table->luma_weight_l0_flag.size());
  EXPECT_THAT(pred_weight_table->luma_weight_l0_flag,
              ::testing::ElementsAreArray({0}));
  EXPECT_EQ(0, pred_weight_table->luma_weight_l0.size());
  EXPECT_EQ(0, pred_weight_table->luma_offset_l0.size());
  EXPECT_EQ(1, pred_weight_table->chroma_weight_l0_flag.size());
  EXPECT_THAT(pred_weight_table->chroma_weight_l0_flag,
              ::testing::ElementsAreArray({0}));
  EXPECT_EQ(0, pred_weight_table->chroma_weight_l0.size());
  EXPECT_EQ(0, pred_weight_table->chroma_offset_l0.size());
  EXPECT_EQ(0, pred_weight_table->luma_weight_l1_flag.size());
  EXPECT_EQ(0, pred_weight_table->luma_weight_l1.size());
  EXPECT_EQ(0, pred_weight_table->luma_offset_l1.size());
  EXPECT_EQ(0, pred_weight_table->chroma_weight_l1_flag.size());
  EXPECT_EQ(0, pred_weight_table->chroma_weight_l1.size());
  EXPECT_EQ(0, pred_weight_table->chroma_offset_l1.size());
}

}  // namespace h264nal
