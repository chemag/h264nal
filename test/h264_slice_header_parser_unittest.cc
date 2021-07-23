/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_slice_header_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h264_bitstream_parser_state.h"
#include "h264_common.h"
#include "h264_dec_ref_pic_marking_parser.h"
#include "h264_pred_weight_table_parser.h"
#include "h264_pps_parser.h"
#include "h264_ref_pic_list_reordering_parser.h"
#include "h264_sps_parser.h"

#include "rtc_base/arraysize.h"
#include "rtc_base/bit_buffer.h"

namespace h264nal {

class H264SliceHeaderParserTest : public ::testing::Test {
 public:
  H264SliceHeaderParserTest() {}
  ~H264SliceHeaderParserTest() override {}
};

#ifdef NOTDEF
TEST_F(H264SliceHeaderParserTest, TestSampleSlice) {
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      0xaf, 0x09, 0x40, 0xf3, 0xb8, 0xd5, 0x39, 0xba,
      0x1f, 0xe4, 0xa6, 0x08, 0x5c, 0x6e, 0xb1, 0x8f,
      0x00, 0x38, 0xf1, 0xa6, 0xfc, 0xf1, 0x40, 0x04,
      0x3a, 0x86, 0xcb, 0x90, 0x74, 0xce, 0xf0, 0x46,
      0x61, 0x93, 0x72, 0xd6, 0xfc, 0x35, 0xe3, 0xc5
  };

  // fuzzer::conv: begin
  // get some mock state
  H264BitstreamParserState bitstream_parser_state;
  auto sps = std::make_shared<H264SpsParser::SpsState>();
  sps->sample_adaptive_offset_enabled_flag = 1;
  sps->chroma_format_idc = 1;
  bitstream_parser_state.sps[0] = sps;
  auto pps = std::make_shared<H264PpsParser::PpsState>();
  bitstream_parser_state.pps[0] = pps;

  auto slice_layer =
      H264SliceHeaderParser::ParseSliceHeader(
          buffer, arraysize(buffer), NalUnitType::IDR_W_RADL,
          &bitstream_parser_state);
  // fuzzer::conv: end

  EXPECT_TRUE(slice_layer != nullptr);

  auto& slice_header = slice_layer->slice_header;

  EXPECT_EQ(1, slice_header->first_slice_in_pic_flag);
  EXPECT_EQ(0, slice_header->no_output_of_prior_pics_flag);
  EXPECT_EQ(0, slice_header->slice_pic_parameter_set_id);
  EXPECT_EQ(2, slice_header->slice_type);
  EXPECT_EQ(1, slice_header->slice_sao_luma_flag);
  EXPECT_EQ(1, slice_header->slice_sao_chroma_flag);
  EXPECT_EQ(9, slice_header->slice_qp_delta);

#if 0
  EXPECT_EQ(1, slice_header->alignment_bit_equal_to_one);
  EXPECT_EQ(0, slice_header->alignment_bit_equal_to_zero);
  EXPECT_EQ(0, slice_header->alignment_bit_equal_to_zero);
  EXPECT_EQ(0, slice_header->alignment_bit_equal_to_zero);
  EXPECT_EQ(0, slice_header->alignment_bit_equal_to_zero);
  EXPECT_EQ(0, slice_header->alignment_bit_equal_to_zero);
  EXPECT_EQ(0, slice_header->alignment_bit_equal_to_zero);
#endif
}

TEST_F(H264SliceHeaderParserTest, TestSampleSlice2) {
  const uint8_t buffer[] = {
      0x26, 0x01, 0x20, 0xf3, 0xc3, 0x5c, 0xfd, 0xfe,
      0xd6, 0x25, 0xbc, 0x0d, 0xb1, 0xfa, 0x81, 0x63,
      0xde, 0x0a, 0xc4, 0xa7, 0xea, 0x42, 0x89, 0xb6,
      0x1e, 0xbb, 0x5e, 0x3f, 0xfd, 0x6c, 0x8a, 0x2d
  };

  // get some mock state
  H264BitstreamParserState bitstream_parser_state;
  auto sps = std::make_shared<H264SpsParser::SpsState>();
  sps->sample_adaptive_offset_enabled_flag = 1;
  sps->chroma_format_idc = 1;
  sps->log2_min_luma_coding_block_size_minus3 = 0;
  sps->log2_diff_max_min_luma_coding_block_size = 2;
  sps->pic_width_in_luma_samples = 1280;
  sps->pic_height_in_luma_samples = 736;
  bitstream_parser_state.sps[0] = sps;
  auto pps = std::make_shared<H264PpsParser::PpsState>();
  bitstream_parser_state.pps[0] = pps;

  auto slice_layer =
      H264SliceHeaderParser::ParseSliceHeader(
          buffer, arraysize(buffer), NalUnitType::IDR_W_RADL,
          &bitstream_parser_state);
  EXPECT_TRUE(slice_layer != nullptr);

  auto& slice_header = slice_layer->slice_header;

  EXPECT_EQ(0, slice_header->first_slice_in_pic_flag);
  EXPECT_EQ(0, slice_header->no_output_of_prior_pics_flag);
  EXPECT_EQ(0, slice_header->slice_pic_parameter_set_id);
  EXPECT_EQ(192, slice_header->slice_address);
  EXPECT_EQ(3, slice_header->slice_type);
  EXPECT_EQ(1, slice_header->slice_sao_luma_flag);
  EXPECT_EQ(0, slice_header->slice_sao_chroma_flag);
  EXPECT_EQ(15, slice_header->slice_qp_delta);

#if 0
  EXPECT_EQ(1, slice_header->alignment_bit_equal_to_one);
  EXPECT_EQ(0, slice_header->alignment_bit_equal_to_zero);
  EXPECT_EQ(0, slice_header->alignment_bit_equal_to_zero);
  EXPECT_EQ(0, slice_header->alignment_bit_equal_to_zero);
  EXPECT_EQ(0, slice_header->alignment_bit_equal_to_zero);
  EXPECT_EQ(0, slice_header->alignment_bit_equal_to_zero);
  EXPECT_EQ(0, slice_header->alignment_bit_equal_to_zero);
#endif
}

TEST_F(H264SliceHeaderParserTest, TestSampleSliceBroken) {
  const uint8_t buffer[] = {
			0x02, 0x01, 0xd2, 0x78, 0xf7, 0x55, 0xdc, 0xbf,
			0x2c, 0x44, 0x00, 0x41, 0xa8, 0xd1, 0x66, 0x44,
			0xb3, 0x12, 0x93, 0xbc, 0x96, 0x54, 0x62, 0x42,
			0x53, 0xee, 0xd6, 0x14, 0x3a, 0xf4, 0x39, 0xfd,
			0x9a, 0x61, 0x28, 0xe2, 0x52, 0x7b, 0x50, 0x48,
			0x4d, 0x1c, 0xe3, 0xc5, 0x63, 0xe6, 0x7d, 0xa5,
			0xb0, 0x87, 0xd4, 0x5f, 0x8a, 0x07, 0x4d, 0x99,
			0x40, 0xcc, 0xe6, 0x21, 0xe3, 0x48, 0xc0, 0x06,
			0x33, 0x2c, 0x0c, 0x04, 0x77, 0x39, 0x22, 0xb6,
			0x7b, 0xe1, 0x40, 0x59, 0xec, 0x19, 0x04, 0xfc,
			0x9a, 0xa5, 0x3f, 0x3a, 0x80, 0x5a, 0x62, 0x3d,
			0xcf, 0xc1, 0x38, 0xe3, 0x8c, 0xed, 0xf1, 0x44,
			0xa8, 0x54, 0xe9, 0x16, 0xf1, 0x22, 0x7b, 0x86,
			0x3a, 0x8a, 0x7f, 0x9b, 0xac, 0x64, 0xe6, 0xa2,
			0xd7, 0x0f, 0x3d, 0x6e, 0xe8, 0xc2, 0x0d, 0xa7,
			0x48, 0xbd, 0x55, 0x53, 0x71, 0x89, 0x8b, 0x82,
			0x59, 0x5c, 0xce, 0x18, 0x1e, 0xa5, 0x9f, 0xcd,
			0x1a, 0xdd, 0x1e, 0x81, 0x69, 0x3d, 0x2d, 0x2d,
			0xb2, 0xab, 0x29, 0xdc, 0xdf, 0x65, 0x28, 0x48,
			0x34, 0x46, 0x01, 0x56, 0xdd, 0xae, 0x93, 0x19,
  };

  // get some mock state
  H264BitstreamParserState bitstream_parser_state;
  auto sps = std::make_shared<H264SpsParser::SpsState>();
  sps->sample_adaptive_offset_enabled_flag = 1;
  sps->chroma_format_idc = 1;
  sps->log2_min_luma_coding_block_size_minus3 = 0;
  sps->log2_diff_max_min_luma_coding_block_size = 2;
  sps->pic_width_in_luma_samples = 1280;
  sps->pic_height_in_luma_samples = 736;
  bitstream_parser_state.sps[0] = sps;
  auto pps = std::make_shared<H264PpsParser::PpsState>();
  bitstream_parser_state.pps[0] = pps;

  auto slice_layer =
      H264SliceHeaderParser::ParseSliceHeader(
          buffer, arraysize(buffer), NalUnitType::IDR_W_RADL,
          &bitstream_parser_state);
  EXPECT_TRUE(slice_layer != nullptr);

  // invalid slice_header
  auto& slice_header = slice_layer->slice_header;
  EXPECT_TRUE(slice_header == nullptr);
}
#endif  // NOTDEF

}  // namespace h264nal
