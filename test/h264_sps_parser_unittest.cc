/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_sps_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h264_common.h"
#include "rtc_common.h"

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

  // seq_parameter_set_data()
  auto& sps_data = sps->sps_data;
  EXPECT_TRUE(sps_data != nullptr);
  EXPECT_EQ(66, sps_data->profile_idc);
  EXPECT_EQ(1, sps_data->constraint_set0_flag);
  EXPECT_EQ(1, sps_data->constraint_set1_flag);
  EXPECT_EQ(0, sps_data->constraint_set2_flag);
  EXPECT_EQ(0, sps_data->constraint_set3_flag);
  EXPECT_EQ(0, sps_data->constraint_set4_flag);
  EXPECT_EQ(0, sps_data->constraint_set5_flag);
  EXPECT_EQ(0, sps_data->reserved_zero_2bits);
  EXPECT_EQ(22, sps_data->level_idc);
  EXPECT_EQ(0, sps_data->seq_parameter_set_id);
  // Verify chroma_format_idc defaults to 1 for Baseline profile (fix for issue #7)
  EXPECT_EQ(1, sps_data->chroma_format_idc);
  EXPECT_EQ(1, sps_data->log2_max_frame_num_minus4);
  EXPECT_EQ(2, sps_data->pic_order_cnt_type);
  EXPECT_EQ(16, sps_data->max_num_ref_frames);
  EXPECT_EQ(0, sps_data->gaps_in_frame_num_value_allowed_flag);
  EXPECT_EQ(19, sps_data->pic_width_in_mbs_minus1);
  EXPECT_EQ(14, sps_data->pic_height_in_map_units_minus1);
  EXPECT_EQ(1, sps_data->frame_mbs_only_flag);
  EXPECT_EQ(1, sps_data->direct_8x8_inference_flag);
  EXPECT_EQ(0, sps_data->frame_cropping_flag);
  EXPECT_EQ(1, sps_data->vui_parameters_present_flag);

  // vui_parameters()
  auto& vui_parameters = sps_data->vui_parameters;
  EXPECT_TRUE(vui_parameters != nullptr);
  EXPECT_EQ(0, vui_parameters->aspect_ratio_info_present_flag);
  EXPECT_EQ(0, vui_parameters->overscan_info_present_flag);
  EXPECT_EQ(1, vui_parameters->video_signal_type_present_flag);
  EXPECT_EQ(5, vui_parameters->video_format);
  EXPECT_EQ(1, vui_parameters->video_full_range_flag);
  EXPECT_EQ(0, vui_parameters->colour_description_present_flag);
  EXPECT_EQ(0, vui_parameters->chroma_loc_info_present_flag);
  EXPECT_EQ(1, vui_parameters->timing_info_present_flag);
  EXPECT_EQ(1, vui_parameters->num_units_in_tick);
  EXPECT_EQ(50, vui_parameters->time_scale);
  EXPECT_EQ(0, vui_parameters->fixed_frame_rate_flag);
  EXPECT_EQ(0, vui_parameters->nal_hrd_parameters_present_flag);
  EXPECT_EQ(0, vui_parameters->vcl_hrd_parameters_present_flag);
  EXPECT_EQ(0, vui_parameters->pic_struct_present_flag);
  EXPECT_EQ(1, vui_parameters->bitstream_restriction_flag);
  EXPECT_EQ(1, vui_parameters->motion_vectors_over_pic_boundaries_flag);
  EXPECT_EQ(0, vui_parameters->max_bytes_per_pic_denom);
  EXPECT_EQ(0, vui_parameters->max_bits_per_mb_denom);
  EXPECT_EQ(10, vui_parameters->log2_max_mv_length_horizontal);
  EXPECT_EQ(10, vui_parameters->log2_max_mv_length_vertical);
  EXPECT_EQ(0, vui_parameters->max_num_reorder_frames);
  EXPECT_EQ(16, vui_parameters->max_dec_frame_buffering);

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

  // seq_parameter_set_data()
  auto& sps_data = sps->sps_data;
  EXPECT_TRUE(sps_data != nullptr);
  EXPECT_EQ(100, sps_data->profile_idc);
  EXPECT_EQ(0, sps_data->constraint_set0_flag);
  EXPECT_EQ(0, sps_data->constraint_set1_flag);
  EXPECT_EQ(0, sps_data->constraint_set2_flag);
  EXPECT_EQ(0, sps_data->constraint_set3_flag);
  EXPECT_EQ(0, sps_data->constraint_set4_flag);
  EXPECT_EQ(0, sps_data->constraint_set5_flag);
  EXPECT_EQ(0, sps_data->reserved_zero_2bits);
  EXPECT_EQ(51, sps_data->level_idc);
  EXPECT_EQ(0, sps_data->seq_parameter_set_id);
  EXPECT_EQ(1, sps_data->chroma_format_idc);

  EXPECT_EQ(0, sps_data->bit_depth_luma_minus8);
  EXPECT_EQ(0, sps_data->bit_depth_chroma_minus8);
  EXPECT_EQ(0, sps_data->qpprime_y_zero_transform_bypass_flag);
  EXPECT_EQ(0, sps_data->seq_scaling_matrix_present_flag);
  EXPECT_EQ(2, sps_data->log2_max_frame_num_minus4);
  EXPECT_EQ(0, sps_data->pic_order_cnt_type);
  EXPECT_EQ(4, sps_data->log2_max_pic_order_cnt_lsb_minus4);
  EXPECT_EQ(16, sps_data->max_num_ref_frames);
  EXPECT_EQ(0, sps_data->gaps_in_frame_num_value_allowed_flag);
  EXPECT_EQ(119, sps_data->pic_width_in_mbs_minus1);
  EXPECT_EQ(67, sps_data->pic_height_in_map_units_minus1);
  EXPECT_EQ(1, sps_data->frame_mbs_only_flag);
  EXPECT_EQ(1, sps_data->direct_8x8_inference_flag);
  EXPECT_EQ(1, sps_data->frame_cropping_flag);
  EXPECT_EQ(0, sps_data->frame_crop_left_offset);
  EXPECT_EQ(0, sps_data->frame_crop_right_offset);
  EXPECT_EQ(0, sps_data->frame_crop_top_offset);
  EXPECT_EQ(4, sps_data->frame_crop_bottom_offset);
  EXPECT_EQ(1, sps_data->vui_parameters_present_flag);

  // vui_parameters()
  auto& vui_parameters = sps_data->vui_parameters;
  EXPECT_TRUE(vui_parameters != nullptr);
  EXPECT_EQ(1, vui_parameters->aspect_ratio_info_present_flag);
  EXPECT_EQ(1, vui_parameters->aspect_ratio_idc);
  EXPECT_EQ(0, vui_parameters->overscan_info_present_flag);
  EXPECT_EQ(0, vui_parameters->video_signal_type_present_flag);
  EXPECT_EQ(0, vui_parameters->chroma_loc_info_present_flag);
  EXPECT_EQ(1, vui_parameters->timing_info_present_flag);
  EXPECT_EQ(1, vui_parameters->num_units_in_tick);
  EXPECT_EQ(60, vui_parameters->time_scale);
  EXPECT_EQ(0, vui_parameters->fixed_frame_rate_flag);
  EXPECT_EQ(0, vui_parameters->nal_hrd_parameters_present_flag);
  EXPECT_EQ(0, vui_parameters->vcl_hrd_parameters_present_flag);
  EXPECT_EQ(0, vui_parameters->pic_struct_present_flag);
  EXPECT_EQ(1, vui_parameters->bitstream_restriction_flag);
  EXPECT_EQ(1, vui_parameters->motion_vectors_over_pic_boundaries_flag);
  EXPECT_EQ(0, vui_parameters->max_bytes_per_pic_denom);
  EXPECT_EQ(0, vui_parameters->max_bits_per_mb_denom);
  EXPECT_EQ(11, vui_parameters->log2_max_mv_length_horizontal);
  EXPECT_EQ(11, vui_parameters->log2_max_mv_length_vertical);
  EXPECT_EQ(2, vui_parameters->max_num_reorder_frames);
  EXPECT_EQ(16, vui_parameters->max_dec_frame_buffering);
}

TEST_F(H264SpsParserTest, TestBaselineProfileWithCropping) {
  // Real SPS from 601_cropped.264 (created with ffmpeg crop filter)
  // This test verifies the fix for GitHub issue #7:
  // chroma_format_idc should default to 1 for non-High profiles
  //
  // Source: 601.264 (320x240, Baseline profile, no cropping)
  // Cropped with: ffmpeg -i 601.264 -vf crop=320:232:0:0 -c:v libx264
  //               -profile:v baseline -pix_fmt yuv420p -an 601_cropped.264
  //
  // Expected SPS values:
  // - profile_idc: 66 (Baseline)
  // - pic_width_in_mbs_minus1: 19 (320 pixels)
  // - pic_height_in_map_units_minus1: 14 (240 pixels)
  // - frame_cropping_flag: 1
  // - frame_crop_bottom_offset: 4
  // - Expected resolution: 320x232 (240 - 2*4 = 232 with CropUnitY=2)
  //
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      0x42, 0xc0, 0x0d, 0xd9, 0x01, 0x41, 0xff, 0x96, 0x6c, 0x80, 0x00, 0x00,
      0x03, 0x00, 0x80, 0x00, 0x00, 0x19, 0x07, 0x8a, 0x15, 0x24, 0x00
  };
  // fuzzer::conv: begin
  auto sps = H264SpsParser::ParseSps(buffer, arraysize(buffer));
  // fuzzer::conv: end

  EXPECT_TRUE(sps != nullptr);

  auto& sps_data = sps->sps_data;
  EXPECT_TRUE(sps_data != nullptr);

  // Verify Baseline profile
  EXPECT_EQ(66, sps_data->profile_idc);
  EXPECT_EQ(ProfileType::CONSTRAINED_BASELINE, sps_data->profile_type);
  EXPECT_EQ(13, sps_data->level_idc);

  // THE FIX: Verify chroma_format_idc defaults to 1 (not 0)
  // For Baseline/Main profiles, chroma_format_idc is not signaled in the
  // bitstream and must default to 1 per H.264 spec 7.4.2.1.1
  EXPECT_EQ(1, sps_data->chroma_format_idc);

  // Verify frame dimensions
  EXPECT_EQ(0, sps_data->log2_max_frame_num_minus4);
  EXPECT_EQ(2, sps_data->pic_order_cnt_type);
  EXPECT_EQ(3, sps_data->max_num_ref_frames);
  EXPECT_EQ(0, sps_data->gaps_in_frame_num_value_allowed_flag);
  EXPECT_EQ(19, sps_data->pic_width_in_mbs_minus1);     // 20 MBs = 320 pixels
  EXPECT_EQ(14, sps_data->pic_height_in_map_units_minus1);  // 15 MBs = 240 pixels
  EXPECT_EQ(1, sps_data->frame_mbs_only_flag);
  EXPECT_EQ(1, sps_data->direct_8x8_inference_flag);

  // Verify frame cropping (this is what triggers the bug!)
  EXPECT_EQ(1, sps_data->frame_cropping_flag);
  EXPECT_EQ(0, sps_data->frame_crop_left_offset);
  EXPECT_EQ(0, sps_data->frame_crop_right_offset);
  EXPECT_EQ(0, sps_data->frame_crop_top_offset);
  EXPECT_EQ(4, sps_data->frame_crop_bottom_offset);

  EXPECT_EQ(1, sps_data->vui_parameters_present_flag);

  // Verify resolution calculation with cropping
  //
  // With the fix (chroma_format_idc=1, 4:2:0 chroma):
  //   - SubHeightC = 2 (from Table 6-1)
  //   - CropUnitY = SubHeightC * (2 - frame_mbs_only_flag) = 2 * 1 = 2
  //   - height = 240 - (CropUnitY * frame_crop_bottom_offset)
  //            = 240 - (2 * 4) = 232 pixels (ok)
  //
  // Without the fix (chroma_format_idc=0, monochrome):
  //   - ChromaArrayType = 0
  //   - CropUnitY = 2 - frame_mbs_only_flag = 1
  //   - height = 240 - (1 * 4) = 236 pixels (wrong -- off by 4 pixels)
  //
  // This is the exact bug reported in GitHub issue #7
  int width = 0;
  int height = 0;
  int result = sps_data->getResolution(&width, &height);

  EXPECT_EQ(0, result);
  EXPECT_EQ(320, width);
  EXPECT_EQ(232, height);  // Would be 236 without the fix
}

}  // namespace h264nal
