/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_slice_header_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h264_bitstream_parser_state.h"
#include "h264_common.h"
#include "h264_dec_ref_pic_marking_parser.h"
#include "h264_pps_parser.h"
#include "h264_pred_weight_table_parser.h"
#include "h264_ref_pic_list_modification_parser.h"
#include "h264_sps_parser.h"
#include "rtc_common.h"

namespace h264nal {

class H264SliceHeaderParserTest : public ::testing::Test {
 public:
  H264SliceHeaderParserTest() {}
  ~H264SliceHeaderParserTest() override {}
};

TEST_F(H264SliceHeaderParserTest, TestSampleSliceIDR601) {
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      0x88, 0x82, 0x06, 0x78, 0x8c, 0x50, 0x00, 0x1c,
      0xab, 0x8e, 0x00, 0x02, 0xfb, 0x31, 0xc0, 0x00,
      0x5f, 0x66, 0xfb, 0xef, 0xbe
  };

  // fuzzer::conv: begin
  // get some mock state
  H264BitstreamParserState bitstream_parser_state;
  auto sps = std::make_shared<H264SpsParser::SpsState>();
  sps->sps_data = std::make_unique<H264SpsDataParser::SpsDataState>();
  sps->sps_data->log2_max_frame_num_minus4 = 1;
  sps->sps_data->frame_mbs_only_flag = 1;
  sps->sps_data->pic_order_cnt_type = 2;
  sps->sps_data->delta_pic_order_always_zero_flag = 0;
  sps->sps_data->pic_width_in_mbs_minus1 = 0;
  sps->sps_data->pic_height_in_map_units_minus1 = 0;
  bitstream_parser_state.sps[0] = sps;
  auto pps = std::make_shared<H264PpsParser::PpsState>();
  pps->bottom_field_pic_order_in_frame_present_flag = 0;
  pps->redundant_pic_cnt_present_flag = 0;
  pps->weighted_pred_flag = 0;
  pps->weighted_bipred_idc = 0;
  pps->entropy_coding_mode_flag = 0;
  pps->deblocking_filter_control_present_flag = 1;
  pps->num_slice_groups_minus1 = 0;
  pps->slice_group_map_type = 0;
  pps->slice_group_change_rate_minus1 = 0;
  bitstream_parser_state.pps[0] = pps;

  uint32_t nal_ref_idc = 3;
  uint32_t nal_unit_type = NalUnitType::CODED_SLICE_OF_IDR_PICTURE_NUT;
  auto slice_header = H264SliceHeaderParser::ParseSliceHeader(
      buffer, arraysize(buffer), nal_ref_idc, nal_unit_type,
      &bitstream_parser_state);
  // fuzzer::conv: end

  EXPECT_TRUE(slice_header != nullptr);

  EXPECT_EQ(0, slice_header->first_mb_in_slice);
  EXPECT_EQ(7, slice_header->slice_type);
  EXPECT_EQ(0, slice_header->pic_parameter_set_id);
  EXPECT_EQ(0, slice_header->frame_num);
  EXPECT_EQ(0, slice_header->field_pic_flag);
  EXPECT_EQ(0, slice_header->bottom_field_flag);
  EXPECT_EQ(0, slice_header->idr_pic_id);
  EXPECT_EQ(0, slice_header->pic_order_cnt_lsb);
  EXPECT_EQ(0, slice_header->delta_pic_order_cnt_bottom);
  EXPECT_EQ(0, slice_header->delta_pic_order_cnt.size());
  EXPECT_EQ(0, slice_header->redundant_pic_cnt);
  EXPECT_EQ(0, slice_header->direct_spatial_mv_pred_flag);
  EXPECT_EQ(0, slice_header->num_ref_idx_active_override_flag);
  EXPECT_EQ(0, slice_header->num_ref_idx_l0_active_minus1);
  EXPECT_EQ(0, slice_header->num_ref_idx_l1_active_minus1);

  EXPECT_NE(nullptr, slice_header->ref_pic_list_modification);
  EXPECT_EQ(0, slice_header->ref_pic_list_modification
                   ->ref_pic_list_modification_flag_l0);
  EXPECT_EQ(0, slice_header->ref_pic_list_modification
                   ->ref_pic_list_modification_flag_l1);
  EXPECT_EQ(0, slice_header->ref_pic_list_modification
                   ->modification_of_pic_nums_idc.size());
  EXPECT_EQ(
      0,
      slice_header->ref_pic_list_modification->abs_diff_pic_num_minus1.size());
  EXPECT_EQ(0,
            slice_header->ref_pic_list_modification->long_term_pic_num.size());

  EXPECT_EQ(nullptr, slice_header->pred_weight_table);

  EXPECT_NE(nullptr, slice_header->dec_ref_pic_marking);
  EXPECT_EQ(0, slice_header->dec_ref_pic_marking->no_output_of_prior_pics_flag);
  EXPECT_EQ(0, slice_header->dec_ref_pic_marking->long_term_reference_flag);
  EXPECT_EQ(
      0, slice_header->dec_ref_pic_marking->adaptive_ref_pic_marking_mode_flag);
  EXPECT_EQ(0, slice_header->dec_ref_pic_marking
                   ->memory_management_control_operation.size());
  EXPECT_EQ(
      0,
      slice_header->dec_ref_pic_marking->difference_of_pic_nums_minus1.size());
  EXPECT_EQ(0, slice_header->dec_ref_pic_marking->long_term_pic_num.size());
  EXPECT_EQ(0, slice_header->dec_ref_pic_marking->long_term_frame_idx.size());
  EXPECT_EQ(
      0,
      slice_header->dec_ref_pic_marking->max_long_term_frame_idx_plus1.size());

  EXPECT_EQ(0, slice_header->cabac_init_idc);
  EXPECT_EQ(0, slice_header->sp_for_switch_flag);
  EXPECT_EQ(-12, slice_header->slice_qp_delta);
  EXPECT_EQ(0, slice_header->slice_qs_delta);
  EXPECT_EQ(0, slice_header->disable_deblocking_filter_idc);
  EXPECT_EQ(0, slice_header->slice_alpha_c0_offset_div2);
  EXPECT_EQ(0, slice_header->slice_beta_offset_div2);
  EXPECT_EQ(0, slice_header->slice_group_change_cycle);
}

TEST_F(H264SliceHeaderParserTest, TestSampleSliceNonIDR601) {
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      0x9a, 0x1c, 0x0c, 0xf0, 0x09, 0x6c
  };

  // fuzzer::conv: begin
  // get some mock state
  H264BitstreamParserState bitstream_parser_state;
  auto sps = std::make_shared<H264SpsParser::SpsState>();
  sps->sps_data = std::make_unique<H264SpsDataParser::SpsDataState>();
  sps->sps_data->log2_max_frame_num_minus4 = 1;
  sps->sps_data->frame_mbs_only_flag = 1;
  sps->sps_data->pic_order_cnt_type = 2;
  sps->sps_data->delta_pic_order_always_zero_flag = 0;
  sps->sps_data->pic_width_in_mbs_minus1 = 0;
  sps->sps_data->pic_height_in_map_units_minus1 = 0;
  bitstream_parser_state.sps[0] = sps;
  auto pps = std::make_shared<H264PpsParser::PpsState>();
  pps->bottom_field_pic_order_in_frame_present_flag = 0;
  pps->redundant_pic_cnt_present_flag = 0;
  pps->weighted_pred_flag = 0;
  pps->weighted_bipred_idc = 0;
  pps->entropy_coding_mode_flag = 0;
  pps->deblocking_filter_control_present_flag = 1;
  pps->num_slice_groups_minus1 = 0;
  pps->slice_group_map_type = 0;
  pps->slice_group_change_rate_minus1 = 0;
  bitstream_parser_state.pps[0] = pps;

  uint32_t nal_ref_idc = 2;
  uint32_t nal_unit_type = NalUnitType::CODED_SLICE_OF_NON_IDR_PICTURE_NUT;
  auto slice_header = H264SliceHeaderParser::ParseSliceHeader(
      buffer, arraysize(buffer), nal_ref_idc, nal_unit_type,
      &bitstream_parser_state);
  // fuzzer::conv: end

  EXPECT_TRUE(slice_header != nullptr);

  EXPECT_EQ(0, slice_header->first_mb_in_slice);
  EXPECT_EQ(5, slice_header->slice_type);
  EXPECT_EQ(0, slice_header->pic_parameter_set_id);
  EXPECT_EQ(1, slice_header->frame_num);
  EXPECT_EQ(0, slice_header->field_pic_flag);
  EXPECT_EQ(0, slice_header->bottom_field_flag);
  EXPECT_EQ(0, slice_header->idr_pic_id);
  EXPECT_EQ(0, slice_header->pic_order_cnt_lsb);
  EXPECT_EQ(0, slice_header->delta_pic_order_cnt_bottom);
  EXPECT_EQ(0, slice_header->delta_pic_order_cnt.size());
  EXPECT_EQ(0, slice_header->redundant_pic_cnt);
  EXPECT_EQ(0, slice_header->direct_spatial_mv_pred_flag);
  EXPECT_EQ(1, slice_header->num_ref_idx_active_override_flag);
  EXPECT_EQ(0, slice_header->num_ref_idx_l0_active_minus1);
  EXPECT_EQ(0, slice_header->num_ref_idx_l1_active_minus1);

  EXPECT_NE(nullptr, slice_header->ref_pic_list_modification);
  EXPECT_EQ(0, slice_header->ref_pic_list_modification
                   ->ref_pic_list_modification_flag_l0);
  EXPECT_EQ(0, slice_header->ref_pic_list_modification
                   ->ref_pic_list_modification_flag_l1);
  EXPECT_EQ(0, slice_header->ref_pic_list_modification
                   ->modification_of_pic_nums_idc.size());
  EXPECT_EQ(
      0,
      slice_header->ref_pic_list_modification->abs_diff_pic_num_minus1.size());
  EXPECT_EQ(0,
            slice_header->ref_pic_list_modification->long_term_pic_num.size());

  EXPECT_EQ(nullptr, slice_header->pred_weight_table);

  EXPECT_NE(nullptr, slice_header->dec_ref_pic_marking);
  EXPECT_EQ(0, slice_header->dec_ref_pic_marking->no_output_of_prior_pics_flag);
  EXPECT_EQ(0, slice_header->dec_ref_pic_marking->long_term_reference_flag);
  EXPECT_EQ(
      0, slice_header->dec_ref_pic_marking->adaptive_ref_pic_marking_mode_flag);
  EXPECT_EQ(0, slice_header->dec_ref_pic_marking
                   ->memory_management_control_operation.size());
  EXPECT_EQ(
      0,
      slice_header->dec_ref_pic_marking->difference_of_pic_nums_minus1.size());
  EXPECT_EQ(0, slice_header->dec_ref_pic_marking->long_term_pic_num.size());
  EXPECT_EQ(0, slice_header->dec_ref_pic_marking->long_term_frame_idx.size());
  EXPECT_EQ(
      0,
      slice_header->dec_ref_pic_marking->max_long_term_frame_idx_plus1.size());

  EXPECT_EQ(0, slice_header->cabac_init_idc);
  EXPECT_EQ(0, slice_header->sp_for_switch_flag);
  EXPECT_EQ(-12, slice_header->slice_qp_delta);
  EXPECT_EQ(0, slice_header->slice_qs_delta);
  EXPECT_EQ(0, slice_header->disable_deblocking_filter_idc);
  EXPECT_EQ(0, slice_header->slice_alpha_c0_offset_div2);
  EXPECT_EQ(0, slice_header->slice_beta_offset_div2);
  EXPECT_EQ(0, slice_header->slice_group_change_cycle);
}

TEST_F(H264SliceHeaderParserTest, TestDeltaPicOrderCntBothEntries) {
  // First non-IDR slice of the JVT conformance stream "mr9_bt_b.h264", a
  // 192x128 PAFF sequence with pic_order_cnt_type 1 and
  // bottom_field_pic_order_in_frame_present_flag 1.
  //
  // Section 7.3.3 reads delta_pic_order_cnt[1] when
  // bottom_field_pic_order_in_frame_present_flag is set and the picture is
  // not a field. The slice header used to copy that flag from the PPS only
  // inside the pic_order_cnt_type == 0 branch, so for pic_order_cnt_type 1
  // it was still 0 and the second se(v) was never consumed. Every frame
  // picture then desynced from that point on; field pictures, which do not
  // carry delta_pic_order_cnt[1], were unaffected.
  //
  // Only the leading bytes of the slice are needed: the parser stops at the
  // end of the slice header.
  // fuzzer::conv: data
  const uint8_t buffer[] = {
      0xe2, 0xf4, 0xaf, 0xff, 0xf0, 0x18, 0x0e, 0xf8,
      0x1e, 0xd1, 0x0a, 0xff, 0x0a, 0x74, 0xa7, 0x66,
      0xd3, 0x1a, 0x36, 0x80, 0x71, 0x6e, 0xac, 0x10
  };

  // fuzzer::conv: begin
  // get some mock state
  H264BitstreamParserState bitstream_parser_state;
  auto sps = std::make_shared<H264SpsParser::SpsState>();
  sps->sps_data = std::make_unique<H264SpsDataParser::SpsDataState>();
  sps->sps_data->log2_max_frame_num_minus4 = 0;
  sps->sps_data->frame_mbs_only_flag = 0;
  sps->sps_data->pic_order_cnt_type = 1;
  sps->sps_data->delta_pic_order_always_zero_flag = 0;
  sps->sps_data->pic_width_in_mbs_minus1 = 11;
  sps->sps_data->pic_height_in_map_units_minus1 = 3;
  bitstream_parser_state.sps[0] = sps;
  auto pps = std::make_shared<H264PpsParser::PpsState>();
  pps->bottom_field_pic_order_in_frame_present_flag = 1;
  pps->redundant_pic_cnt_present_flag = 0;
  pps->weighted_pred_flag = 0;
  pps->weighted_bipred_idc = 0;
  pps->entropy_coding_mode_flag = 1;
  pps->deblocking_filter_control_present_flag = 0;
  pps->num_slice_groups_minus1 = 0;
  pps->slice_group_map_type = 0;
  pps->slice_group_change_rate_minus1 = 0;
  pps->num_ref_idx_l0_default_active_minus1 = 4;
  pps->num_ref_idx_l1_default_active_minus1 = 0;
  bitstream_parser_state.pps[0] = pps;

  uint32_t nal_ref_idc = 2;
  uint32_t nal_unit_type = NalUnitType::CODED_SLICE_OF_NON_IDR_PICTURE_NUT;
  auto slice_header = H264SliceHeaderParser::ParseSliceHeader(
      buffer, arraysize(buffer), nal_ref_idc, nal_unit_type,
      &bitstream_parser_state);
  // fuzzer::conv: end

  ASSERT_TRUE(slice_header != nullptr);

  EXPECT_EQ(0, slice_header->first_mb_in_slice);
  EXPECT_EQ(0, slice_header->slice_type);
  EXPECT_EQ(0, slice_header->pic_parameter_set_id);
  EXPECT_EQ(1, slice_header->frame_num);
  EXPECT_EQ(0, slice_header->field_pic_flag);
  // the flag is now visible outside the pic_order_cnt_type == 0 branch
  EXPECT_EQ(1, slice_header->bottom_field_pic_order_in_frame_present_flag);
  // both entries, not just delta_pic_order_cnt[0]
  ASSERT_EQ(2u, slice_header->delta_pic_order_cnt.size());
  EXPECT_EQ(0, slice_header->delta_pic_order_cnt[0]);
  EXPECT_EQ(0, slice_header->delta_pic_order_cnt[1]);
  // everything below here was desynced by the missing se(v)
  EXPECT_EQ(1, slice_header->num_ref_idx_active_override_flag);
  EXPECT_EQ(0, slice_header->num_ref_idx_l0_active_minus1);

  EXPECT_NE(nullptr, slice_header->ref_pic_list_modification);
  EXPECT_EQ(0, slice_header->ref_pic_list_modification
                   ->ref_pic_list_modification_flag_l0);
  EXPECT_EQ(0, slice_header->ref_pic_list_modification
                   ->ref_pic_list_modification_flag_l1);

  // adaptive marking with MMCO opcodes: a sensitive desync canary
  EXPECT_NE(nullptr, slice_header->dec_ref_pic_marking);
  EXPECT_EQ(
      1, slice_header->dec_ref_pic_marking->adaptive_ref_pic_marking_mode_flag);
  ASSERT_EQ(2u, slice_header->dec_ref_pic_marking
                    ->memory_management_control_operation.size());
  EXPECT_EQ(
      4, slice_header->dec_ref_pic_marking
             ->memory_management_control_operation[0]);
  EXPECT_EQ(
      0, slice_header->dec_ref_pic_marking
             ->memory_management_control_operation[1]);
  ASSERT_EQ(1u, slice_header->dec_ref_pic_marking
                    ->max_long_term_frame_idx_plus1.size());
  EXPECT_EQ(
      2, slice_header->dec_ref_pic_marking->max_long_term_frame_idx_plus1[0]);

  EXPECT_EQ(0, slice_header->cabac_init_idc);
  EXPECT_EQ(0, slice_header->slice_qp_delta);
}

}  // namespace h264nal
