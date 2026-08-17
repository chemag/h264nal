/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_vui_parameters_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h264_common.h"
#include "rtc_common.h"

namespace h264nal {

class H264VuiParametersParserTest : public ::testing::Test {
 public:
  H264VuiParametersParserTest() {}
  ~H264VuiParametersParserTest() override {}
};

TEST_F(H264VuiParametersParserTest, TestSampleVuiParameters601) {
  // VUI (601.264)
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      0x36, 0x40, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00,
      0x0c, 0x83, 0xc5, 0x8b, 0x84, 0x60
  };
  // fuzzer::conv: begin
  auto vui_parameters =
      H264VuiParametersParser::ParseVuiParameters(buffer, arraysize(buffer));
  // fuzzer::conv: end

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

TEST_F(H264VuiParametersParserTest, TestSampleVuiParameters601vui) {
  // VUI (601vui.264)
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      0x37, 0x06, 0x06, 0x06, 0x40, 0x00, 0x00, 0x00,
      0x40, 0x00, 0x00, 0x0c, 0x83, 0xc5, 0x8b, 0x84,
      0x60, 0x00
  };
  // fuzzer::conv: begin
  auto vui_parameters =
      H264VuiParametersParser::ParseVuiParameters(buffer, arraysize(buffer));
  // fuzzer::conv: end

  EXPECT_TRUE(vui_parameters != nullptr);

  EXPECT_EQ(0, vui_parameters->aspect_ratio_info_present_flag);
  EXPECT_EQ(0, vui_parameters->overscan_info_present_flag);
  EXPECT_EQ(1, vui_parameters->video_signal_type_present_flag);
  EXPECT_EQ(5, vui_parameters->video_format);
  EXPECT_EQ(1, vui_parameters->video_full_range_flag);
  EXPECT_EQ(1, vui_parameters->colour_description_present_flag);
  EXPECT_EQ(6, vui_parameters->colour_primaries);
  EXPECT_EQ(6, vui_parameters->transfer_characteristics);
  EXPECT_EQ(6, vui_parameters->matrix_coefficients);
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

TEST_F(H264VuiParametersParserTest, TestSampleVuiParameters709) {
  // VUI (709.264)
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      0x37, 0x01, 0x01, 0x01, 0x40, 0x00, 0x00, 0x00,
      0x40, 0x00, 0x00, 0x0c, 0x83, 0xc5, 0x8b, 0x84,
      0x60, 0x00
  };
  // fuzzer::conv: begin
  auto vui_parameters =
      H264VuiParametersParser::ParseVuiParameters(buffer, arraysize(buffer));
  // fuzzer::conv: end

  EXPECT_TRUE(vui_parameters != nullptr);

  EXPECT_EQ(0, vui_parameters->aspect_ratio_info_present_flag);
  EXPECT_EQ(0, vui_parameters->overscan_info_present_flag);
  EXPECT_EQ(1, vui_parameters->video_signal_type_present_flag);
  EXPECT_EQ(5, vui_parameters->video_format);
  EXPECT_EQ(1, vui_parameters->video_full_range_flag);
  EXPECT_EQ(1, vui_parameters->colour_description_present_flag);
  EXPECT_EQ(1, vui_parameters->colour_primaries);
  EXPECT_EQ(1, vui_parameters->transfer_characteristics);
  EXPECT_EQ(1, vui_parameters->matrix_coefficients);
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

TEST_F(H264VuiParametersParserTest, TestSampleVuiParameters709vui) {
  // VUI (709vui.264)
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      0x37, 0x01, 0x01, 0x01, 0x40, 0x00, 0x00, 0x00,
      0x40, 0x00, 0x00, 0x0c, 0x83, 0xc5, 0x8b, 0x84,
      0x60, 0x00
  };
  // fuzzer::conv: begin
  auto vui_parameters =
      H264VuiParametersParser::ParseVuiParameters(buffer, arraysize(buffer));
  // fuzzer::conv: end

  EXPECT_TRUE(vui_parameters != nullptr);

  EXPECT_EQ(0, vui_parameters->aspect_ratio_info_present_flag);
  EXPECT_EQ(0, vui_parameters->overscan_info_present_flag);
  EXPECT_EQ(1, vui_parameters->video_signal_type_present_flag);
  EXPECT_EQ(5, vui_parameters->video_format);
  EXPECT_EQ(1, vui_parameters->video_full_range_flag);
  EXPECT_EQ(1, vui_parameters->colour_description_present_flag);
  EXPECT_EQ(1, vui_parameters->colour_primaries);
  EXPECT_EQ(1, vui_parameters->transfer_characteristics);
  EXPECT_EQ(1, vui_parameters->matrix_coefficients);
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

TEST_F(H264VuiParametersParserTest, TestSampleVuiParametersSmallMaxDecFrameBuffering) {
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      0x37, 0x05, 0x06, 0x05, 0x40, 0x39, 0xcc, 0x66,
      0xce, 0xe6, 0xb2, 0x80, 0x23, 0x68, 0x50, 0x9a,
      0x80
  };
  // fuzzer::conv: begin
  auto vui_parameters =
      H264VuiParametersParser::ParseVuiParameters(buffer, arraysize(buffer));
  // fuzzer::conv: end

  EXPECT_TRUE(vui_parameters != nullptr);

  EXPECT_EQ(0, vui_parameters->aspect_ratio_info_present_flag);
  EXPECT_EQ(0, vui_parameters->overscan_info_present_flag);
  EXPECT_EQ(1, vui_parameters->video_signal_type_present_flag);
  EXPECT_EQ(5, vui_parameters->video_format);
  EXPECT_EQ(1, vui_parameters->video_full_range_flag);
  EXPECT_EQ(1, vui_parameters->colour_description_present_flag);
  EXPECT_EQ(5, vui_parameters->colour_primaries);
  EXPECT_EQ(6, vui_parameters->transfer_characteristics);
  EXPECT_EQ(5, vui_parameters->matrix_coefficients);
  EXPECT_EQ(0, vui_parameters->chroma_loc_info_present_flag);
  EXPECT_EQ(1, vui_parameters->timing_info_present_flag);
  EXPECT_EQ(15151515, vui_parameters->num_units_in_tick);
  EXPECT_EQ(1000000000, vui_parameters->time_scale);
  EXPECT_EQ(1, vui_parameters->fixed_frame_rate_flag);
  EXPECT_EQ(0, vui_parameters->nal_hrd_parameters_present_flag);
  EXPECT_EQ(0, vui_parameters->vcl_hrd_parameters_present_flag);
  EXPECT_EQ(0, vui_parameters->pic_struct_present_flag);
  EXPECT_EQ(1, vui_parameters->bitstream_restriction_flag);
  EXPECT_EQ(1, vui_parameters->motion_vectors_over_pic_boundaries_flag);
  EXPECT_EQ(2, vui_parameters->max_bytes_per_pic_denom);
  EXPECT_EQ(1, vui_parameters->max_bits_per_mb_denom);
  EXPECT_EQ(9, vui_parameters->log2_max_mv_length_horizontal);
  EXPECT_EQ(8, vui_parameters->log2_max_mv_length_vertical);
  EXPECT_EQ(0, vui_parameters->max_num_reorder_frames);
  EXPECT_EQ(1, vui_parameters->max_dec_frame_buffering);
}

// The 3 tests below share a minimal VUI whose only content is the timing
// info: the 4 leading present flags are 0, timing_info_present_flag is 1,
// and what follows is num_units_in_tick u(32), time_scale u(32) and
// fixed_frame_rate_flag. They differ only in those two 32 bit fields.

TEST_F(H264VuiParametersParserTest, TestZeroNumUnitsInTick) {
  // num_units_in_tick = 0, time_scale = 60000. Section E.2.1 requires
  // num_units_in_tick to be greater than 0; it is the denominator of
  // equation D-2, so a zero divides by zero in getFramerate().
  const uint8_t buffer[] = {0x08, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x07, 0x53, 0x00, 0x00};
  auto vui_parameters =
      H264VuiParametersParser::ParseVuiParameters(buffer, arraysize(buffer));
  EXPECT_TRUE(vui_parameters == nullptr);
}

TEST_F(H264VuiParametersParserTest, TestZeroTimeScale) {
  // num_units_in_tick = 1001, time_scale = 0. Section E.2.1 requires
  // time_scale to be greater than 0 as well, which makes the framerate 0
  // rather than undefined, but is just as invalid.
  const uint8_t buffer[] = {0x08, 0x00, 0x00, 0x1f, 0x48,
                            0x00, 0x00, 0x00, 0x00, 0x00};
  auto vui_parameters =
      H264VuiParametersParser::ParseVuiParameters(buffer, arraysize(buffer));
  EXPECT_TRUE(vui_parameters == nullptr);
}

TEST_F(H264VuiParametersParserTest, TestValidTimingInfo) {
  // num_units_in_tick = 1001, time_scale = 60000, the usual 29.97 fps
  // pair. Shows the two checks do not reject a legal timing info, and is
  // the only coverage getFramerate() has.
  const uint8_t buffer[] = {0x08, 0x00, 0x00, 0x1f, 0x48,
                            0x00, 0x07, 0x53, 0x00, 0x00};
  auto vui_parameters =
      H264VuiParametersParser::ParseVuiParameters(buffer, arraysize(buffer));
  ASSERT_TRUE(vui_parameters != nullptr);

  EXPECT_EQ(1, vui_parameters->timing_info_present_flag);
  EXPECT_EQ(1001, vui_parameters->num_units_in_tick);
  EXPECT_EQ(60000, vui_parameters->time_scale);
  // equation D-2: 60000 / (2 * 1001)
  EXPECT_NEAR(29.97003, vui_parameters->getFramerate(), 0.00001);
}

}  // namespace h264nal
