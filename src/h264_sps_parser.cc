/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_sps_parser.h"


#include <cinttypes>
#include <stdio.h>

#include <cmath>
#include <cstdint>
#include <memory>
#include <vector>

#include "h264_common.h"
#include "h264_vui_parameters_parser.h"

namespace h264nal {

// General note: this is based off the 2012 version of the H.264 standard.
// You can find it on this page:
// http://www.itu.int/rec/T-REC-H.264

// Unpack RBSP and parse SPS data state from the supplied buffer.
std::unique_ptr<H264SpsDataParser::SpsDataState>
H264SpsDataParser::ParseSpsData(const uint8_t* data, size_t length) noexcept {
  std::vector<uint8_t> unpacked_buffer = UnescapeRbsp(data, length);
  BitBuffer bit_buffer(unpacked_buffer.data(), unpacked_buffer.size());
  return ParseSpsData(&bit_buffer);
}

std::unique_ptr<H264SpsDataParser::SpsDataState>
H264SpsDataParser::ParseSpsData(BitBuffer* bit_buffer) noexcept {
  uint32_t bits_tmp;
  int32_t golomb_tmp;

  // H264 SPS Nal Unit (seq_parameter_set_data(()) parser.
  // Section 7.3.2.1.1 ("Sequence parameter set data syntax") of the H.264
  // standard for a complete description.
  auto sps_data = std::make_unique<SpsDataState>();

  // profile_idc  u(8)
  if (!bit_buffer->ReadBits(8, sps_data->profile_idc)) {
    return nullptr;
  }

  // constraint_set0_flag  u(1)
  if (!bit_buffer->ReadBits(1, sps_data->constraint_set0_flag)) {
    return nullptr;
  }

  // constraint_set1_flag  u(1)
  if (!bit_buffer->ReadBits(1, sps_data->constraint_set1_flag)) {
    return nullptr;
  }

  // constraint_set2_flag  u(1)
  if (!bit_buffer->ReadBits(1, sps_data->constraint_set2_flag)) {
    return nullptr;
  }

  // constraint_set3_flag  u(1)
  if (!bit_buffer->ReadBits(1, sps_data->constraint_set3_flag)) {
    return nullptr;
  }

  // constraint_set4_flag  u(1)
  if (!bit_buffer->ReadBits(1, sps_data->constraint_set4_flag)) {
    return nullptr;
  }

  // constraint_set5_flag  u(1)
  if (!bit_buffer->ReadBits(1, sps_data->constraint_set5_flag)) {
    return nullptr;
  }

  // reserved_zero_2bits  u(2)
  if (!bit_buffer->ReadBits(2, sps_data->reserved_zero_2bits)) {
    return nullptr;
  }

  // derive the profile
  sps_data->profile_type = sps_data->GetProfileType();

  // level_idc  u(8)
  if (!bit_buffer->ReadBits(8, sps_data->level_idc)) {
    return nullptr;
  }

  // seq_parameter_set_id  ue(v)
  if (!bit_buffer->ReadExponentialGolomb(sps_data->seq_parameter_set_id)) {
    return nullptr;
  }
  if (sps_data->seq_parameter_set_id < kSeqParameterSetIdMin ||
      sps_data->seq_parameter_set_id > kSeqParameterSetIdMax) {
#ifdef FPRINT_ERRORS
    fprintf(stderr,
            "invalid seq_parameter_set_id: %" PRIu32
            " not in range "
            "[%" PRIu32 ", %" PRIu32 "]\n",
            sps_data->seq_parameter_set_id, kSeqParameterSetIdMin,
            kSeqParameterSetIdMax);
#endif  // FPRINT_ERRORS
    return nullptr;
  }

  if (sps_data->profile_idc == 100 || sps_data->profile_idc == 110 ||
      sps_data->profile_idc == 122 || sps_data->profile_idc == 244 ||
      sps_data->profile_idc == 44 || sps_data->profile_idc == 83 ||
      sps_data->profile_idc == 86 || sps_data->profile_idc == 118 ||
      sps_data->profile_idc == 128 || sps_data->profile_idc == 138 ||
      sps_data->profile_idc == 139 || sps_data->profile_idc == 134 ||
      sps_data->profile_idc == 135) {
    // chroma_format_idc  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(sps_data->chroma_format_idc)) {
      return nullptr;
    }
    if (sps_data->chroma_format_idc < kChromaFormatIdcMin ||
        sps_data->chroma_format_idc > kChromaFormatIdcMax) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid chroma_format_idc: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              sps_data->chroma_format_idc, kChromaFormatIdcMin,
              kChromaFormatIdcMax);
#endif  // FPRINT_ERRORS
      return nullptr;
    }

    if (sps_data->chroma_format_idc == 3) {
      // separate_colour_plane_flag  u(1)
      if (!bit_buffer->ReadBits(1, sps_data->separate_colour_plane_flag)) {
        return nullptr;
      }
    }

    // bit_depth_luma_minus8  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(sps_data->bit_depth_luma_minus8)) {
      return nullptr;
    }
    if (sps_data->bit_depth_luma_minus8 < kBitDepthLumaMinus8Min ||
        sps_data->bit_depth_luma_minus8 > kBitDepthLumaMinus8Max) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid bit_depth_luma_minus8: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              sps_data->bit_depth_luma_minus8, kBitDepthLumaMinus8Min,
              kBitDepthLumaMinus8Max);
#endif  // FPRINT_ERRORS
      return nullptr;
    }

    // bit_depth_chroma_minus8  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(sps_data->bit_depth_chroma_minus8)) {
      return nullptr;
    }
    if (sps_data->bit_depth_chroma_minus8 < kBitDepthChromaMinus8Min ||
        sps_data->bit_depth_chroma_minus8 > kBitDepthChromaMinus8Max) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid bit_depth_chroma_minus8: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              sps_data->bit_depth_chroma_minus8, kBitDepthChromaMinus8Min,
              kBitDepthChromaMinus8Max);
#endif  // FPRINT_ERRORS
      return nullptr;
    }

    // qpprime_y_zero_transform_bypass_flag  u(1)
    if (!bit_buffer->ReadBits(1,
                              sps_data->qpprime_y_zero_transform_bypass_flag)) {
      return nullptr;
    }

    // seq_scaling_matrix_present_flag  u(1)
    if (!bit_buffer->ReadBits(1, sps_data->seq_scaling_matrix_present_flag)) {
      return nullptr;
    }

    if (sps_data->seq_scaling_matrix_present_flag) {
      for (uint32_t i = 0; i < ((sps_data->chroma_format_idc != 3) ? 8 : 12);
           i++) {
        // seq_scaling_list_present_flag[i]  u(1)
        if (!bit_buffer->ReadBits(1, bits_tmp)) {
          return nullptr;
        }
        sps_data->seq_scaling_list_present_flag.push_back(bits_tmp);

        if (sps_data->seq_scaling_list_present_flag[i]) {
          // scaling_list()
          if (i < 6) {
            (void)sps_data->scaling_list(
                bit_buffer, i, sps_data->ScalingList4x4, 16,
                sps_data->UseDefaultScalingMatrix4x4Flag);
          } else {
            (void)sps_data->scaling_list(
                bit_buffer, i - 6, sps_data->ScalingList8x8, 64,
                sps_data->UseDefaultScalingMatrix4x4Flag);
          }
        }
      }
    }
  }

  // log2_max_frame_num_minus4  ue(v)
  if (!bit_buffer->ReadExponentialGolomb(sps_data->log2_max_frame_num_minus4)) {
    return nullptr;
  }
  if (sps_data->log2_max_frame_num_minus4 < kLog2MaxFrameNumMinus4Min ||
      sps_data->log2_max_frame_num_minus4 > kLog2MaxFrameNumMinus4Max) {
#ifdef FPRINT_ERRORS
    fprintf(stderr,
            "invalid log2_max_frame_num_minus4: %" PRIu32
            " not in range "
            "[%" PRIu32 ", %" PRIu32 "]\n",
            sps_data->log2_max_frame_num_minus4, kLog2MaxFrameNumMinus4Min,
            kLog2MaxFrameNumMinus4Max);
#endif  // FPRINT_ERRORS
    return nullptr;
  }

  // pic_order_cnt_type  ue(v)
  if (!bit_buffer->ReadExponentialGolomb(sps_data->pic_order_cnt_type)) {
    return nullptr;
  }
  if (sps_data->pic_order_cnt_type < kPicOrderCntTypeMin ||
      sps_data->pic_order_cnt_type > kPicOrderCntTypeMax) {
#ifdef FPRINT_ERRORS
    fprintf(stderr,
            "invalid pic_order_cnt_type: %" PRIu32
            " not in range "
            "[%" PRIu32 ", %" PRIu32 "]\n",
            sps_data->pic_order_cnt_type, kPicOrderCntTypeMin,
            kPicOrderCntTypeMax);
#endif  // FPRINT_ERRORS
    return nullptr;
  }

  if (sps_data->pic_order_cnt_type == 0) {
    // log2_max_pic_order_cnt_lsb_minus4  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(
            sps_data->log2_max_pic_order_cnt_lsb_minus4)) {
      return nullptr;
    }
    if (sps_data->log2_max_pic_order_cnt_lsb_minus4 <
            kLog2MaxPicOrderCntLsbMinus4Min ||
        sps_data->log2_max_pic_order_cnt_lsb_minus4 >
            kLog2MaxPicOrderCntLsbMinus4Max) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid log2_max_pic_order_cnt_lsb_minus4: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              sps_data->log2_max_pic_order_cnt_lsb_minus4,
              kLog2MaxPicOrderCntLsbMinus4Min, kLog2MaxPicOrderCntLsbMinus4Max);
#endif  // FPRINT_ERRORS
      return nullptr;
    }

  } else if (sps_data->pic_order_cnt_type == 1) {
    // delta_pic_order_always_zero_flag  u(1)
    if (!bit_buffer->ReadBits(1, sps_data->delta_pic_order_always_zero_flag)) {
      return nullptr;
    }

    // offset_for_non_ref_pic  se(v)
    if (!bit_buffer->ReadSignedExponentialGolomb(
            sps_data->offset_for_non_ref_pic)) {
      return nullptr;
    }

    // offset_for_top_to_bottom_field  se(v)
    if (!bit_buffer->ReadSignedExponentialGolomb(
            sps_data->offset_for_top_to_bottom_field)) {
      return nullptr;
    }

    // num_ref_frames_in_pic_order_cnt_cycle  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(
            sps_data->num_ref_frames_in_pic_order_cnt_cycle)) {
      return nullptr;
    }
    if (sps_data->num_ref_frames_in_pic_order_cnt_cycle <
            kNumRefFramesInPicOrderCntCycleMin ||
        sps_data->num_ref_frames_in_pic_order_cnt_cycle >
            kNumRefFramesInPicOrderCntCycleMax) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid num_ref_frames_in_pic_order_cnt_cycle: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              sps_data->num_ref_frames_in_pic_order_cnt_cycle,
              kNumRefFramesInPicOrderCntCycleMin,
              kNumRefFramesInPicOrderCntCycleMax);
#endif  // FPRINT_ERRORS
      return nullptr;
    }

    for (uint32_t i = 0; i < sps_data->num_ref_frames_in_pic_order_cnt_cycle;
         i++) {
      // offset_for_ref_frame[i]  se(v)
      if (!bit_buffer->ReadSignedExponentialGolomb(golomb_tmp)) {
        return nullptr;
      }
      sps_data->offset_for_ref_frame.push_back(golomb_tmp);
    }
  }

  // max_num_ref_frames  ue(v)
  if (!bit_buffer->ReadExponentialGolomb(sps_data->max_num_ref_frames)) {
    return nullptr;
  }
  if (sps_data->max_num_ref_frames > H264VuiParametersParser::kMaxDpbFrames) {
#ifdef FPRINT_ERRORS
    fprintf(stderr,
            "invalid max_num_ref_frames: %" PRIu32
            " not in range "
            "[%" PRIu32 ", %" PRIu32 "]\n",
            sps_data->max_num_ref_frames, 0,
            H264VuiParametersParser::kMaxDpbFrames);
#endif  // FPRINT_ERRORS
    return nullptr;
  }

  // gaps_in_frame_num_value_allowed_flag  u(1)
  if (!bit_buffer->ReadBits(1,
                            sps_data->gaps_in_frame_num_value_allowed_flag)) {
    return nullptr;
  }

  // pic_width_in_mbs_minus1  ue(v)
  if (!bit_buffer->ReadExponentialGolomb(sps_data->pic_width_in_mbs_minus1)) {
    return nullptr;
  }
  if (sps_data->pic_width_in_mbs_minus1 > kMaxMbWidth) {
#ifdef FPRINT_ERRORS
    fprintf(stderr,
            "invalid pic_width_in_mbs_minus1: %" PRIu32
            " not in range "
            "[%" PRIu32 ", %" PRIu32 "]\n",
            sps_data->pic_width_in_mbs_minus1, 0, kMaxMbWidth);
#endif  // FPRINT_ERRORS
    return nullptr;
  }

  // pic_height_in_map_units_minus1  ue(v)
  if (!bit_buffer->ReadExponentialGolomb(
          sps_data->pic_height_in_map_units_minus1)) {
    return nullptr;
  }
  if (sps_data->pic_height_in_map_units_minus1 > kMaxMbHeight) {
#ifdef FPRINT_ERRORS
    fprintf(stderr,
            "invalid pic_height_in_map_units_minus1: %" PRIu32
            " not in range "
            "[%" PRIu32 ", %" PRIu32 "]\n",
            sps_data->pic_height_in_map_units_minus1, 0, kMaxMbHeight);
#endif  // FPRINT_ERRORS
    return nullptr;
  }

  // frame_mbs_only_flag  u(1)
  if (!bit_buffer->ReadBits(1, sps_data->frame_mbs_only_flag)) {
    return nullptr;
  }

  if (!sps_data->frame_mbs_only_flag) {
    // mb_adaptive_frame_field_flag  u(1)
    if (!bit_buffer->ReadBits(1, sps_data->mb_adaptive_frame_field_flag)) {
      return nullptr;
    }
  }

  // direct_8x8_inference_flag  u(1)
  if (!bit_buffer->ReadBits(1, sps_data->direct_8x8_inference_flag)) {
    return nullptr;
  }

  // frame_cropping_flag  u(1)
  if (!bit_buffer->ReadBits(1, sps_data->frame_cropping_flag)) {
    return nullptr;
  }

  if (sps_data->frame_cropping_flag) {
    // frame_crop_left_offset  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(sps_data->frame_crop_left_offset)) {
      return nullptr;
    }
    if (sps_data->frame_crop_left_offset > kMaxWidth) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid frame_crop_left_offset: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              sps_data->frame_crop_left_offset, 0, kMaxWidth);
#endif  // FPRINT_ERRORS
      return nullptr;
    }

    // frame_crop_right_offset  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(sps_data->frame_crop_right_offset)) {
      return nullptr;
    }
    if (sps_data->frame_crop_right_offset > kMaxWidth) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid frame_crop_right_offset: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              sps_data->frame_crop_right_offset, 0, kMaxWidth);
#endif  // FPRINT_ERRORS
      return nullptr;
    }

    // frame_crop_top_offset  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(sps_data->frame_crop_top_offset)) {
      return nullptr;
    }
    if (sps_data->frame_crop_top_offset > kMaxHeight) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid frame_crop_top_offset: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              sps_data->frame_crop_top_offset, 0, kMaxHeight);
#endif  // FPRINT_ERRORS
      return nullptr;
    }

    // frame_crop_bottom_offset  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(
            sps_data->frame_crop_bottom_offset)) {
      return nullptr;
    }
    if (sps_data->frame_crop_bottom_offset > kMaxHeight) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid frame_crop_bottom_offset: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              sps_data->frame_crop_bottom_offset, 0, kMaxHeight);
#endif  // FPRINT_ERRORS
      return nullptr;
    }
  }

  // vui_parameters_present_flag  u(1)
  if (!bit_buffer->ReadBits(1, (sps_data->vui_parameters_present_flag))) {
    return nullptr;
  }

  if (sps_data->vui_parameters_present_flag) {
    // vui_parameters()
    sps_data->vui_parameters =
        H264VuiParametersParser::ParseVuiParameters(bit_buffer);
    if (sps_data->vui_parameters == nullptr) {
      return nullptr;
    }
  }

  return sps_data;
}

ProfileType H264SpsDataParser::SpsDataState::GetProfileType() const noexcept {
  switch (profile_idc) {
    case 66:  // Baseline profiles
      if (constraint_set1_flag == 1) {
        return ProfileType::CONSTRAINED_BASELINE;
      }
      return ProfileType::BASELINE;

    case 77:
      return ProfileType::MAIN;

    case 88:
      return ProfileType::EXTENDED;

    case 100:  // High profiles
      if (constraint_set4_flag == 1) {
        return ProfileType::PROGRESSIVE_HIGH;
      }
      if (constraint_set5_flag == 1) {
        return ProfileType::CONSTRAINED_HIGH;
      }
      return ProfileType::HIGH;

    case 110:
      if (constraint_set3_flag == 1) {
        return ProfileType::HIGH_10_INTRA;
      }
      if (constraint_set4_flag == 1) {
        return ProfileType::PROGRESSIVE_HIGH_10;
      }
      return ProfileType::HIGH_10;

    case 122:
      if (constraint_set3_flag == 1) {
        return ProfileType::HIGH_422_INTRA;
      }
      return ProfileType::HIGH_422;

    case 144:
      if (constraint_set3_flag == 1) {
        return ProfileType::HIGH_444_INTRA;
      }
      return ProfileType::HIGH_444;

    case 44:
      return ProfileType::CAVLC_444_INTRA;

    default:
      return ProfileType::UNSPECIFIED;
  }
}

uint32_t H264SpsDataParser::SpsDataState::getChromaArrayType() const noexcept {
  // Rec. ITU-T H.264 (2012) Page 74, Section 7.4.2.1.1
  // the value of the variable ChromaArrayType is assigned as follows:
  // - If separate_colour_plane_flag is equal to 0, ChromaArrayType is set
  //   equal to chroma_format_idc.
  // - Otherwise (separate_colour_plane_flag is equal to 1), ChromaArrayType
  // is set equal to 0.
  uint32_t ChromaArrayType = 0;
  if (separate_colour_plane_flag == 0) {
    ChromaArrayType = chroma_format_idc;
  } else {
    ChromaArrayType = 0;
  }
  return ChromaArrayType;
}

int H264SpsDataParser::SpsDataState::getSubWidthC() const noexcept {
  // Table 6-1
  if (chroma_format_idc == 0 && separate_colour_plane_flag == 0) {
    // monochrome
    return -1;
  } else if (chroma_format_idc == 1 && separate_colour_plane_flag == 0) {
    // 4:2:0
    return 2;
  } else if (chroma_format_idc == 2 && separate_colour_plane_flag == 0) {
    // 4:2:2
    return 2;
  } else if (chroma_format_idc == 3 && separate_colour_plane_flag == 0) {
    // 4:4:4
    return 1;
  } else if (chroma_format_idc == 3 && separate_colour_plane_flag == 1) {
    // 4:4:0
    return -1;
  }
  return -1;
}

int H264SpsDataParser::SpsDataState::getSubHeightC() const noexcept {
  // Table 6-1
  if (chroma_format_idc == 0 && separate_colour_plane_flag == 0) {
    // monochrome
    return -1;
  } else if (chroma_format_idc == 1 && separate_colour_plane_flag == 0) {
    // 4:2:0
    return 2;
  } else if (chroma_format_idc == 2 && separate_colour_plane_flag == 0) {
    // 4:2:2
    return 1;
  } else if (chroma_format_idc == 3 && separate_colour_plane_flag == 0) {
    // 4:4:4
    return 1;
  } else if (chroma_format_idc == 3 && separate_colour_plane_flag == 1) {
    // 4:4:0
    return -1;
  }
  return -1;
}

int H264SpsDataParser::SpsDataState::getResolution(int* width,
                                                   int* height) const noexcept {
  if (width == nullptr || height == nullptr) {
    return -1;
  }
  int ChromaArrayType = getChromaArrayType();
  int CropUnitX = -1;
  int CropUnitY = -1;
  if (ChromaArrayType == 0) {
    // Equations 7-19, 7-20
    CropUnitX = 1;
    CropUnitY = 2 - frame_mbs_only_flag;
  } else {
    // Equations 7-21, 7-22
    int SubWidthC = getSubWidthC();
    int SubHeightC = getSubHeightC();
    CropUnitX = SubWidthC;
    CropUnitY = SubHeightC * (2 - frame_mbs_only_flag);
  }

  *width = 16 * (pic_width_in_mbs_minus1 + 1);
  *height = 16 * (pic_height_in_map_units_minus1 + 1);
  *width -= (CropUnitX * frame_crop_left_offset +
             CropUnitX * frame_crop_right_offset);
  *height -= (CropUnitY * frame_crop_top_offset +
              CropUnitY * frame_crop_bottom_offset);
  return 0;
}

// Section 7.3.2.1.1.1
bool H264SpsDataParser::SpsDataState::scaling_list(
    BitBuffer* bit_buffer, uint32_t i, std::vector<uint32_t>& scalingList,
    uint32_t sizeOfScalingList,
    std::vector<uint32_t>& useDefaultScalingMatrixFlag) noexcept {
  uint32_t lastScale = 8;
  uint32_t nextScale = 8;
  for (uint32_t j = 0; j < sizeOfScalingList; j++) {
    if (nextScale != 0) {
      // delta_scale  se(v)
      if (!bit_buffer->ReadSignedExponentialGolomb(delta_scale)) {
        return false;
      }
      nextScale = (lastScale + (delta_scale) + 256) % 256;
      // make sure vector has ith element
      while (useDefaultScalingMatrixFlag.size() <= i) {
        useDefaultScalingMatrixFlag.push_back(0);
      }
      useDefaultScalingMatrixFlag[i] = (j == 0 && nextScale == 0);
    }
    // make sure vector has jth element
    while (scalingList.size() <= j) {
      scalingList.push_back(0);
    }
    scalingList[j] = (nextScale == 0) ? lastScale : nextScale;
    lastScale = scalingList[j];
  }
  return true;
}

// Unpack RBSP and parse SPS state from the supplied buffer.
std::shared_ptr<H264SpsParser::SpsState> H264SpsParser::ParseSps(
    const uint8_t* data, size_t length) noexcept {
  std::vector<uint8_t> unpacked_buffer = UnescapeRbsp(data, length);
  BitBuffer bit_buffer(unpacked_buffer.data(), unpacked_buffer.size());
  return ParseSps(&bit_buffer);
}

std::shared_ptr<H264SpsParser::SpsState> H264SpsParser::ParseSps(
    BitBuffer* bit_buffer) noexcept {
  // H264 SPS Nal Unit (seq_parameter_set_rbsp(()) parser.
  // Section 7.3.2.1 ("Sequence parameter set RBSP syntax") of the H.264
  // standard for a complete description.
  auto sps = std::make_shared<SpsState>();

  // seq_parameter_set_data()
  sps->sps_data = H264SpsDataParser::ParseSpsData(bit_buffer);
  if (sps->sps_data == nullptr) {
    return nullptr;
  }

  rbsp_trailing_bits(bit_buffer);

  return sps;
}

uint32_t H264SpsParser::SpsState::getChromaArrayType() noexcept {
  return sps_data->getChromaArrayType();
}

#ifdef FDUMP_DEFINE
void H264SpsDataParser::SpsDataState::fdump(
    FILE* outfp, int indent_level, ParsingOptions parsing_options) const {
  fprintf(outfp, "sps_data {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "profile_idc: %i", profile_idc);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "constraint_set0_flag: %i", constraint_set0_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "constraint_set1_flag: %i", constraint_set1_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "constraint_set2_flag: %i", constraint_set2_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "constraint_set3_flag: %i", constraint_set3_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "constraint_set4_flag: %i", constraint_set4_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "constraint_set5_flag: %i", constraint_set5_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "reserved_zero_2bits: %i", reserved_zero_2bits);

  fdump_indent_level(outfp, indent_level);
  std::string profile_type_str;
  profileTypeToString(profile_type, profile_type_str);
  fprintf(outfp, "profile: %s", profile_type_str.c_str());

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "level_idc: %i", level_idc);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "seq_parameter_set_id: %i", seq_parameter_set_id);

  if (profile_idc == 100 || profile_idc == 110 || profile_idc == 122 ||
      profile_idc == 244 || profile_idc == 44 || profile_idc == 83 ||
      profile_idc == 86 || profile_idc == 118 || profile_idc == 128 ||
      profile_idc == 138 || profile_idc == 139 || profile_idc == 134 ||
      profile_idc == 135) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "chroma_format_idc: %i", chroma_format_idc);

    if (chroma_format_idc == 3) {
      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "separate_colour_plane_flag: %i",
              separate_colour_plane_flag);
    }

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "bit_depth_luma_minus8: %i", bit_depth_luma_minus8);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "bit_depth_chroma_minus8: %i", bit_depth_chroma_minus8);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "qpprime_y_zero_transform_bypass_flag: %i",
            qpprime_y_zero_transform_bypass_flag);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "seq_scaling_matrix_present_flag: %i",
            seq_scaling_matrix_present_flag);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "seq_scaling_list_present_flag {");
    for (const uint32_t& v : seq_scaling_list_present_flag) {
      fprintf(outfp, " %i", v);
    }
    fprintf(outfp, " }");

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "ScalingList4x4 {");
    for (const uint32_t& v : ScalingList4x4) {
      fprintf(outfp, " %i", v);
    }
    fprintf(outfp, " }");

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "UseDefaultScalingMatrix4x4Flag {");
    for (const uint32_t& v : UseDefaultScalingMatrix4x4Flag) {
      fprintf(outfp, " %i", v);
    }
    fprintf(outfp, " }");

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "ScalingList8x8 {");
    for (const uint32_t& v : ScalingList8x8) {
      fprintf(outfp, " %i", v);
    }
    fprintf(outfp, " }");

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "UseDefaultScalingMatrix8x8Flag {");
    for (const uint32_t& v : UseDefaultScalingMatrix8x8Flag) {
      fprintf(outfp, " %i", v);
    }
    fprintf(outfp, " }");

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "delta_scale: %i", delta_scale);
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "log2_max_frame_num_minus4: %i", log2_max_frame_num_minus4);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "pic_order_cnt_type: %i", pic_order_cnt_type);

  if (pic_order_cnt_type == 0) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "log2_max_pic_order_cnt_lsb_minus4: %i",
            log2_max_pic_order_cnt_lsb_minus4);

  } else if (pic_order_cnt_type == 1) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "delta_pic_order_always_zero_flag: %i",
            delta_pic_order_always_zero_flag);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "offset_for_non_ref_pic: %i", offset_for_non_ref_pic);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "offset_for_top_to_bottom_field: %i",
            offset_for_top_to_bottom_field);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "num_ref_frames_in_pic_order_cnt_cycle: %i",
            num_ref_frames_in_pic_order_cnt_cycle);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "offset_for_ref_frame {");
    for (const int32_t& v : offset_for_ref_frame) {
      fprintf(outfp, " %i", v);
    }
    fprintf(outfp, " }");
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "max_num_ref_frames: %i", max_num_ref_frames);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "gaps_in_frame_num_value_allowed_flag: %i",
          gaps_in_frame_num_value_allowed_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "pic_width_in_mbs_minus1: %i", pic_width_in_mbs_minus1);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "pic_height_in_map_units_minus1: %i",
          pic_height_in_map_units_minus1);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "frame_mbs_only_flag: %i", frame_mbs_only_flag);

  if (!frame_mbs_only_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "mb_adaptive_frame_field_flag: %i",
            mb_adaptive_frame_field_flag);
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "direct_8x8_inference_flag: %i", direct_8x8_inference_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "frame_cropping_flag: %i", frame_cropping_flag);

  if (frame_cropping_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "frame_crop_left_offset: %i", frame_crop_left_offset);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "frame_crop_right_offset: %i", frame_crop_right_offset);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "frame_crop_top_offset: %i", frame_crop_top_offset);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "frame_crop_bottom_offset: %i", frame_crop_bottom_offset);
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "vui_parameters_present_flag: %i",
          vui_parameters_present_flag);

  if (vui_parameters_present_flag) {
    fdump_indent_level(outfp, indent_level);
    vui_parameters->fdump(outfp, indent_level);
  }

  if (parsing_options.add_resolution) {
    // add video resolution
    int width = -1;
    int height = -1;
    getResolution(&width, &height);
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "width: %i", width);
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "height: %i", height);
  }

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}

void H264SpsParser::SpsState::fdump(FILE* outfp, int indent_level,
                                    ParsingOptions parsing_options) const {
  fprintf(outfp, "sps {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  sps_data->fdump(outfp, indent_level, parsing_options);

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}

#endif  // FDUMP_DEFINE

}  // namespace h264nal
