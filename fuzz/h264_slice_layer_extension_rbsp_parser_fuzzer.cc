/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

// This file was auto-generated using fuzz/converter.py from
// h264_slice_layer_extension_rbsp_parser_unittest.cc.
// Do not edit directly.

#include "h264_slice_layer_extension_rbsp_parser.h"
#include "h264_bitstream_parser_state.h"
#include "h264_common.h"
#include "h264_dec_ref_pic_marking_parser.h"
#include "h264_pps_parser.h"
#include "h264_pred_weight_table_parser.h"
#include "h264_ref_pic_list_modification_parser.h"
#include "h264_sps_parser.h"
#include "h264_sps_svc_extension_parser.h"
#include "h264_subset_sps_parser.h"
#include "rtc_common.h"


// libfuzzer infra to test the fuzz target
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  {
  // get some mock state
  h264nal::H264BitstreamParserState bitstream_parser_state;
  auto sps = std::make_shared<h264nal::H264SpsParser::SpsState>();
  sps->sps_data = std::make_unique<h264nal::H264SpsDataParser::SpsDataState>();
  sps->sps_data->profile_idc = 66;
  sps->sps_data->seq_parameter_set_id = 0;
  sps->sps_data->log2_max_frame_num_minus4 = 1;
  sps->sps_data->frame_mbs_only_flag = 1;
  sps->sps_data->pic_order_cnt_type = 2;
  sps->sps_data->delta_pic_order_always_zero_flag = 0;
  sps->sps_data->pic_width_in_mbs_minus1 = 10;
  sps->sps_data->pic_height_in_map_units_minus1 = 8;
  bitstream_parser_state.sps[0] = sps;
  auto pps = std::make_shared<h264nal::H264PpsParser::PpsState>();
  pps->pic_parameter_set_id = 0;
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
  auto pps2 = std::make_shared<h264nal::H264PpsParser::PpsState>();
  pps2->pic_parameter_set_id = 1;
  pps2->bottom_field_pic_order_in_frame_present_flag = 0;
  pps2->redundant_pic_cnt_present_flag = 0;
  pps2->weighted_pred_flag = 0;
  pps2->weighted_bipred_idc = 0;
  pps2->entropy_coding_mode_flag = 0;
  pps2->deblocking_filter_control_present_flag = 1;
  pps2->num_slice_groups_minus1 = 0;
  pps2->slice_group_map_type = 0;
  pps2->slice_group_change_rate_minus1 = 0;
  bitstream_parser_state.pps[1] = pps2;
  auto subset_sps = std::make_shared<h264nal::H264SubsetSpsParser::SubsetSpsState>();
  subset_sps->seq_parameter_set_data =
      std::make_unique<h264nal::H264SpsDataParser::SpsDataState>();
  subset_sps->seq_parameter_set_data->profile_idc = 83;
  subset_sps->seq_parameter_set_data->seq_parameter_set_id = 0;
  subset_sps->seq_parameter_set_data->log2_max_frame_num_minus4 = 1;
  subset_sps->seq_parameter_set_data->frame_mbs_only_flag = 1;
  subset_sps->seq_parameter_set_data->pic_order_cnt_type = 2;
  subset_sps->seq_parameter_set_data->delta_pic_order_always_zero_flag = 0;
  subset_sps->seq_parameter_set_data->pic_width_in_mbs_minus1 = 21;
  subset_sps->seq_parameter_set_data->pic_height_in_map_units_minus1 = 17;
  subset_sps->seq_parameter_set_svc_extension =
      std::make_unique<h264nal::H264SpsSvcExtensionParser::SpsSvcExtensionState>();
  subset_sps->seq_parameter_set_svc_extension
      ->inter_layer_deblocking_filter_control_present_flag = 1;
  subset_sps->seq_parameter_set_svc_extension->slice_header_restriction_flag =
      1;
  bitstream_parser_state.subset_sps[0] = subset_sps;
  h264nal::H264NalUnitHeaderParser::NalUnitHeaderState nal_unit_header;
  nal_unit_header.nal_ref_idc = 3;
  nal_unit_header.nal_unit_type = h264nal::NalUnitType::CODED_SLICE_EXTENSION;
  nal_unit_header.svc_extension_flag = 1;
  nal_unit_header.avc_3d_extension_flag = 0;
  nal_unit_header.nal_unit_header_svc_extension = std::make_unique<
      h264nal::H264NalUnitHeaderSvcExtensionParser::NalUnitHeaderSvcExtensionState>();
  auto& nal_unit_header_svc_extension =
      nal_unit_header.nal_unit_header_svc_extension;
  nal_unit_header_svc_extension->idr_flag = 1;
  nal_unit_header_svc_extension->priority_id = 0;
  nal_unit_header_svc_extension->no_inter_layer_pred_flag = 1;
  nal_unit_header_svc_extension->dependency_id = 1;
  nal_unit_header_svc_extension->quality_id = 0;
  nal_unit_header_svc_extension->temporal_id = 0;
  nal_unit_header_svc_extension->use_ref_base_pic_flag = 0;
  nal_unit_header_svc_extension->discardable_flag = 1;
  nal_unit_header_svc_extension->output_flag = 1;
  nal_unit_header_svc_extension->reserved_three_2bits = 3;
  auto slice_layer_extension_rbsp =
      h264nal::H264SliceLayerExtensionRbspParser::ParseSliceLayerExtensionRbsp(
          data, size, nal_unit_header, &bitstream_parser_state);
  }
  return 0;
}
