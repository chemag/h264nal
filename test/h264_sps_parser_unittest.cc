/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_sps_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h264_common.h"
#include "rtc_base/arraysize.h"
#include "rtc_base/bit_buffer.h"

namespace h264nal {

class H264SpsParserTest : public ::testing::Test {
 public:
  H264SpsParserTest() {}
  ~H264SpsParserTest() override {}
};

TEST_F(H264SpsParserTest, TestSampleSPS601) {
  // SPS (601.264)
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      0x42, 0xc0, 0x16, 0xa6, 0x11, 0x05, 0x07, 0xe9,
      0xb2, 0x00, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00,
      0x03, 0x00, 0x64, 0x1e, 0x2c, 0x5c, 0x23, 0x00
  };
  // fuzzer::conv: begin
  auto sps = H264SpsParser::ParseSps(buffer, arraysize(buffer));
  // fuzzer::conv: end

  EXPECT_TRUE(sps != nullptr);

  EXPECT_EQ(66, sps->profile_idc);
  EXPECT_EQ(1, sps->constraint_set0_flag);
  EXPECT_EQ(1, sps->constraint_set1_flag);
  EXPECT_EQ(0, sps->constraint_set2_flag);
  EXPECT_EQ(0, sps->constraint_set3_flag);
  EXPECT_EQ(0, sps->constraint_set4_flag);
  EXPECT_EQ(0, sps->constraint_set5_flag);
  EXPECT_EQ(0, sps->reserved_zero_2bits);
  EXPECT_EQ(22, sps->level_idc);
  EXPECT_EQ(0, sps->seq_parameter_set_id);
  EXPECT_EQ(1, sps->log2_max_frame_num_minus4);
  EXPECT_EQ(2, sps->pic_order_cnt_type);
  EXPECT_EQ(16, sps->max_num_ref_frames);
  EXPECT_EQ(0, sps->gaps_in_frame_num_value_allowed_flag);
  EXPECT_EQ(19, sps->pic_width_in_mbs_minus1);
  EXPECT_EQ(14, sps->pic_height_in_map_units_minus1);
  EXPECT_EQ(1, sps->frame_mbs_only_flag);
  EXPECT_EQ(1, sps->direct_8x8_inference_flag);
  EXPECT_EQ(0, sps->frame_cropping_flag);
  EXPECT_EQ(1, sps->vui_parameters_present_flag);

  // vui_parameters()
  EXPECT_EQ(0, sps->vui_parameters->aspect_ratio_info_present_flag);
  EXPECT_EQ(0, sps->vui_parameters->overscan_info_present_flag);
  EXPECT_EQ(1, sps->vui_parameters->video_signal_type_present_flag);
  EXPECT_EQ(5, sps->vui_parameters->video_format);
  EXPECT_EQ(1, sps->vui_parameters->video_full_range_flag);
  EXPECT_EQ(0, sps->vui_parameters->colour_description_present_flag);
  EXPECT_EQ(0, sps->vui_parameters->chroma_loc_info_present_flag);
  EXPECT_EQ(1, sps->vui_parameters->timing_info_present_flag);
  EXPECT_EQ(1, sps->vui_parameters->num_units_in_tick);
  EXPECT_EQ(50, sps->vui_parameters->time_scale);
  EXPECT_EQ(0, sps->vui_parameters->fixed_frame_rate_flag);
  EXPECT_EQ(0, sps->vui_parameters->nal_hrd_parameters_present_flag);
  EXPECT_EQ(0, sps->vui_parameters->vcl_hrd_parameters_present_flag);
  EXPECT_EQ(0, sps->vui_parameters->pic_struct_present_flag);
  EXPECT_EQ(1, sps->vui_parameters->bitstream_restriction_flag);
  EXPECT_EQ(1, sps->vui_parameters->motion_vectors_over_pic_boundaries_flag);
  EXPECT_EQ(0, sps->vui_parameters->max_bytes_per_pic_denom);
  EXPECT_EQ(0, sps->vui_parameters->max_bits_per_mb_denom);
  EXPECT_EQ(10, sps->vui_parameters->log2_max_mv_length_horizontal);
  EXPECT_EQ(10, sps->vui_parameters->log2_max_mv_length_vertical);
  EXPECT_EQ(0, sps->vui_parameters->max_num_reorder_frames);
  EXPECT_EQ(16, sps->vui_parameters->max_dec_frame_buffering);

}

TEST_F(H264SpsParserTest, TestSampleSPS2012) {
  // SPS (2012 source)
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      0x64, 0x00, 0x33, 0xac, 0x72, 0x84, 0x40, 0x78,
      0x02, 0x27, 0xe5, 0xc0, 0x44, 0x00, 0x00, 0x03,
      0x00, 0x04, 0x00, 0x00, 0x03, 0x00, 0xf0, 0x3c,
      0x60, 0xc6, 0x11, 0x80
  };
  auto sps = H264SpsParser::ParseSps(buffer, arraysize(buffer));

  EXPECT_TRUE(sps != nullptr);

  EXPECT_EQ(100, sps->profile_idc);
  EXPECT_EQ(0, sps->constraint_set0_flag);
  EXPECT_EQ(0, sps->constraint_set1_flag);
  EXPECT_EQ(0, sps->constraint_set2_flag);
  EXPECT_EQ(0, sps->constraint_set3_flag);
  EXPECT_EQ(0, sps->constraint_set4_flag);
  EXPECT_EQ(0, sps->constraint_set5_flag);
  EXPECT_EQ(0, sps->reserved_zero_2bits);
  EXPECT_EQ(51, sps->level_idc);
  EXPECT_EQ(0, sps->seq_parameter_set_id);
  EXPECT_EQ(1, sps->chroma_format_idc);

  EXPECT_EQ(0, sps->bit_depth_luma_minus8);
  EXPECT_EQ(0, sps->bit_depth_chroma_minus8);
  EXPECT_EQ(0, sps->qpprime_y_zero_transform_bypass_flag);
  EXPECT_EQ(0, sps->seq_scaling_matrix_present_flag);
  EXPECT_EQ(2, sps->log2_max_frame_num_minus4);
  EXPECT_EQ(0, sps->pic_order_cnt_type);
  EXPECT_EQ(4, sps->log2_max_pic_order_cnt_lsb_minus4);
  EXPECT_EQ(16, sps->max_num_ref_frames);
  EXPECT_EQ(0, sps->gaps_in_frame_num_value_allowed_flag);
  EXPECT_EQ(119, sps->pic_width_in_mbs_minus1);
  EXPECT_EQ(67, sps->pic_height_in_map_units_minus1);
  EXPECT_EQ(1, sps->frame_mbs_only_flag);
  EXPECT_EQ(1, sps->direct_8x8_inference_flag);
  EXPECT_EQ(1, sps->frame_cropping_flag);
  EXPECT_EQ(0, sps->frame_crop_left_offset);
  EXPECT_EQ(0, sps->frame_crop_right_offset);
  EXPECT_EQ(0, sps->frame_crop_top_offset);
  EXPECT_EQ(4, sps->frame_crop_bottom_offset);
  EXPECT_EQ(1, sps->vui_parameters_present_flag);

  // vui_parameters()
  EXPECT_EQ(1, sps->vui_parameters->aspect_ratio_info_present_flag);
  EXPECT_EQ(1, sps->vui_parameters->aspect_ratio_idc);
  EXPECT_EQ(0, sps->vui_parameters->overscan_info_present_flag);
  EXPECT_EQ(0, sps->vui_parameters->video_signal_type_present_flag);
  EXPECT_EQ(0, sps->vui_parameters->chroma_loc_info_present_flag);
  EXPECT_EQ(1, sps->vui_parameters->timing_info_present_flag);
  EXPECT_EQ(1, sps->vui_parameters->num_units_in_tick);
  EXPECT_EQ(60, sps->vui_parameters->time_scale);
  EXPECT_EQ(0, sps->vui_parameters->fixed_frame_rate_flag);
  EXPECT_EQ(0, sps->vui_parameters->nal_hrd_parameters_present_flag);
  EXPECT_EQ(0, sps->vui_parameters->vcl_hrd_parameters_present_flag);
  EXPECT_EQ(0, sps->vui_parameters->pic_struct_present_flag);
  EXPECT_EQ(1, sps->vui_parameters->bitstream_restriction_flag);
  EXPECT_EQ(1, sps->vui_parameters->motion_vectors_over_pic_boundaries_flag);
  EXPECT_EQ(0, sps->vui_parameters->max_bytes_per_pic_denom);
  EXPECT_EQ(0, sps->vui_parameters->max_bits_per_mb_denom);
  EXPECT_EQ(11, sps->vui_parameters->log2_max_mv_length_horizontal);
  EXPECT_EQ(11, sps->vui_parameters->log2_max_mv_length_vertical);
  EXPECT_EQ(2, sps->vui_parameters->max_num_reorder_frames);
  EXPECT_EQ(16, sps->vui_parameters->max_dec_frame_buffering);
}

}  // namespace h264nal
