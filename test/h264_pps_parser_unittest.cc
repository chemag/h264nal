/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_pps_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h264_common.h"
#include "rtc_base/arraysize.h"
#include "rtc_base/bit_buffer.h"

namespace h264nal {

class H264PpsParserTest : public ::testing::Test {
 public:
  H264PpsParserTest() {}
  ~H264PpsParserTest() override {}
};

TEST_F(H264PpsParserTest, TestSamplePPS601) {
  // PPS example (601.264)
  // fuzzer::conv: data
  const uint8_t buffer[] = {0xc8, 0x42, 0x02, 0x32, 0xc8};
  // fuzzer::conv: begin
  auto pps = H264PpsParser::ParsePps(buffer, arraysize(buffer));
  // fuzzer::conv: end

  EXPECT_TRUE(pps != nullptr);

  EXPECT_EQ(0, pps->pic_parameter_set_id);
  EXPECT_EQ(0, pps->seq_parameter_set_id);
  EXPECT_EQ(0, pps->entropy_coding_mode_flag);
  EXPECT_EQ(0, pps->pic_order_present_flag);
  EXPECT_EQ(0, pps->num_slice_groups_minus1);
  EXPECT_EQ(15, pps->num_ref_idx_l0_active_minus1);
  EXPECT_EQ(0, pps->num_ref_idx_l1_active_minus1);
  EXPECT_EQ(0, pps->weighted_pred_flag);
  EXPECT_EQ(0, pps->weighted_bipred_idc);
  EXPECT_EQ(-8, pps->pic_init_qp_minus26);
  EXPECT_EQ(0, pps->pic_init_qs_minus26);
  EXPECT_EQ(-2, pps->chroma_qp_index_offset);
  EXPECT_EQ(1, pps->deblocking_filter_control_present_flag);
  EXPECT_EQ(0, pps->constrained_intra_pred_flag);
  EXPECT_EQ(0, pps->redundant_pic_cnt_present_flag);

#if 0
  EXPECT_EQ(1, pps->rbsp_stop_one_bit);
#endif
}

}  // namespace h264nal
