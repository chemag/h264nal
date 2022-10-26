/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_vui_parameters_parser.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "h264_common.h"

namespace h264nal {

// General note: this is based off the 2012 version of the H.264 standard.
// You can find it on this page:
// http://www.itu.int/rec/T-REC-H.264

// Unpack RBSP and parse VUI Parameters state from the supplied buffer.
std::unique_ptr<H264VuiParametersParser::VuiParametersState>
H264VuiParametersParser::ParseVuiParameters(const uint8_t* data,
                                            size_t length) noexcept {
  std::vector<uint8_t> unpacked_buffer = UnescapeRbsp(data, length);
  rtc::BitBuffer bit_buffer(unpacked_buffer.data(), unpacked_buffer.size());
  return ParseVuiParameters(&bit_buffer);
}

std::unique_ptr<H264VuiParametersParser::VuiParametersState>
H264VuiParametersParser::ParseVuiParameters(
    rtc::BitBuffer* bit_buffer) noexcept {
  // H264 vui_parameters() parser.
  // Section E.1 ("VUI parameters syntax") of the H.264 standard for
  // a complete description.
  auto vui = std::make_unique<VuiParametersState>();

  // aspect_ratio_info_present_flag  u(1)
  if (!bit_buffer->ReadBits(1, vui->aspect_ratio_info_present_flag)) {
    return nullptr;
  }

  if (vui->aspect_ratio_info_present_flag) {
    // aspect_ratio_idc  u(8)
    if (!bit_buffer->ReadBits(8, vui->aspect_ratio_idc)) {
      return nullptr;
    }
    if (vui->aspect_ratio_idc == AR_EXTENDED_SAR) {
      // sar_width  u(16)
      if (!bit_buffer->ReadBits(16, vui->sar_width)) {
        return nullptr;
      }
      // sar_height  u(16)
      if (!bit_buffer->ReadBits(16, vui->sar_height)) {
        return nullptr;
      }
    }
  }

  // overscan_info_present_flag  u(1)
  if (!bit_buffer->ReadBits(1, vui->overscan_info_present_flag)) {
    return nullptr;
  }

  if (vui->overscan_info_present_flag) {
    // overscan_appropriate_flag  u(1)
    if (!bit_buffer->ReadBits(1, vui->overscan_appropriate_flag)) {
      return nullptr;
    }
  }

  // video_signal_type_present_flag  u(1)
  if (!bit_buffer->ReadBits(1, vui->video_signal_type_present_flag)) {
    return nullptr;
  }

  if (vui->video_signal_type_present_flag) {
    // video_format  u(3)
    if (!bit_buffer->ReadBits(3, vui->video_format)) {
      return nullptr;
    }
    // video_full_range_flag  u(1)
    if (!bit_buffer->ReadBits(1, vui->video_full_range_flag)) {
      return nullptr;
    }
    // colour_description_present_flag  u(1)
    if (!bit_buffer->ReadBits(1, vui->colour_description_present_flag)) {
      return nullptr;
    }
    if (vui->colour_description_present_flag) {
      // colour_primaries  u(8)
      if (!bit_buffer->ReadBits(8, vui->colour_primaries)) {
        return nullptr;
      }
      // transfer_characteristics  u(8)
      if (!bit_buffer->ReadBits(8, vui->transfer_characteristics)) {
        return nullptr;
      }
      // matrix_coefficients  u(8)
      if (!bit_buffer->ReadBits(8, vui->matrix_coefficients)) {
        return nullptr;
      }
    }
  }

  // chroma_loc_info_present_flag  u(1)
  if (!bit_buffer->ReadBits(1, vui->chroma_loc_info_present_flag)) {
    return nullptr;
  }
  if (vui->chroma_loc_info_present_flag) {
    // chroma_sample_loc_type_top_field  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(
            vui->chroma_sample_loc_type_top_field)) {
      return nullptr;
    }
    if (vui->chroma_sample_loc_type_top_field <
            kChromaSampleLocTypeTopFieldMin ||
        vui->chroma_sample_loc_type_top_field >
            kChromaSampleLocTypeTopFieldMax) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid chroma_sample_loc_type_top_field: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              vui->chroma_sample_loc_type_top_field,
              kChromaSampleLocTypeTopFieldMin, kChromaSampleLocTypeTopFieldMax);
#endif  // FPRINT_ERRORS
      return nullptr;
    }

    // chroma_sample_loc_type_bottom_field  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(
            vui->chroma_sample_loc_type_bottom_field)) {
      return nullptr;
    }
    if (vui->chroma_sample_loc_type_bottom_field <
            kChromaSampleLocTypeBottomFieldMin ||
        vui->chroma_sample_loc_type_bottom_field >
            kChromaSampleLocTypeBottomFieldMax) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid chroma_sample_loc_type_bottom_field: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              vui->chroma_sample_loc_type_bottom_field,
              kChromaSampleLocTypeBottomFieldMin,
              kChromaSampleLocTypeBottomFieldMax);
#endif  // FPRINT_ERRORS
      return nullptr;
    }
  }

  // timing_info_present_flag  u(1)
  if (!bit_buffer->ReadBits(1, vui->timing_info_present_flag)) {
    return nullptr;
  }

  if (vui->timing_info_present_flag) {
    // num_units_in_tick  u(32)
    if (!bit_buffer->ReadBits(32, vui->num_units_in_tick)) {
      return nullptr;
    }
    // time_scale  u(32)
    if (!bit_buffer->ReadBits(32, vui->time_scale)) {
      return nullptr;
    }
    // fixed_frame_rate_flag  u(1)
    if (!bit_buffer->ReadBits(1, vui->fixed_frame_rate_flag)) {
      return nullptr;
    }
  }

  // nal_hrd_parameters_present_flag  u(1)
  if (!bit_buffer->ReadBits(1, vui->nal_hrd_parameters_present_flag)) {
    return nullptr;
  }

  if (vui->nal_hrd_parameters_present_flag) {
    // hrd_parameters()
    vui->nal_hrd_parameters =
        H264HrdParametersParser::ParseHrdParameters(bit_buffer);
    if (vui->nal_hrd_parameters == nullptr) {
      return nullptr;
    }
  }

  // vcl_hrd_parameters_present_flag  u(1)
  if (!bit_buffer->ReadBits(1, vui->vcl_hrd_parameters_present_flag)) {
    return nullptr;
  }

  if (vui->vcl_hrd_parameters_present_flag) {
    // hrd_parameters()
    vui->vcl_hrd_parameters =
        H264HrdParametersParser::ParseHrdParameters(bit_buffer);
    if (vui->vcl_hrd_parameters == nullptr) {
      return nullptr;
    }
  }

  if (vui->nal_hrd_parameters_present_flag ||
      vui->vcl_hrd_parameters_present_flag) {
    // low_delay_hrd_flag  u(1)
    if (!bit_buffer->ReadBits(1, vui->low_delay_hrd_flag)) {
      return nullptr;
    }
  }

  // pic_struct_present_flag  u(1)
  if (!bit_buffer->ReadBits(1, vui->pic_struct_present_flag)) {
    return nullptr;
  }

  // bitstream_restriction_flag  u(1)
  if (!bit_buffer->ReadBits(1, vui->bitstream_restriction_flag)) {
    return nullptr;
  }

  if (vui->bitstream_restriction_flag) {
    // motion_vectors_over_pic_boundaries_flag  u(1)
    if (!bit_buffer->ReadBits(1,
                              vui->motion_vectors_over_pic_boundaries_flag)) {
      return nullptr;
    }
    // max_bytes_per_pic_denom  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(vui->max_bytes_per_pic_denom)) {
      return nullptr;
    }
    if (vui->max_bytes_per_pic_denom < kMaxBytesPerPicDenomMin ||
        vui->max_bytes_per_pic_denom > kMaxBytesPerPicDenomMax) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid max_bytes_per_pic_denom: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              vui->max_bytes_per_pic_denom, kMaxBytesPerPicDenomMin,
              kMaxBytesPerPicDenomMax);
#endif  // FPRINT_ERRORS
      return nullptr;
    }

    // max_bits_per_mb_denom  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(vui->max_bits_per_mb_denom)) {
      return nullptr;
    }
    if (vui->max_bits_per_mb_denom < kMaxBitsPerMbDenomMin ||
        vui->max_bits_per_mb_denom > kMaxBitsPerMbDenomMax) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid max_bits_per_mb_denom: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              vui->max_bits_per_mb_denom, kMaxBitsPerMbDenomMin,
              kMaxBitsPerMbDenomMax);
#endif  // FPRINT_ERRORS
      return nullptr;
    }

    // log2_max_mv_length_horizontal  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(
            vui->log2_max_mv_length_horizontal)) {
      return nullptr;
    }
    if (vui->log2_max_mv_length_horizontal < kLog2MaxMvLengthHorizontalMin ||
        vui->log2_max_mv_length_horizontal > kLog2MaxMvLengthHorizontalMax) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid log2_max_mv_length_horizontal: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              vui->log2_max_mv_length_horizontal, kLog2MaxMvLengthHorizontalMin,
              kLog2MaxMvLengthHorizontalMax);
#endif  // FPRINT_ERRORS
      return nullptr;
    }

    // log2_max_mv_length_vertical  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(vui->log2_max_mv_length_vertical)) {
      return nullptr;
    }
    // max_num_reorder_frames  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(vui->max_num_reorder_frames)) {
      return nullptr;
    }
    // max_dec_frame_buffering  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(vui->max_dec_frame_buffering)) {
      return nullptr;
    }
  }

  return vui;
}

#ifdef FDUMP_DEFINE
void H264VuiParametersParser::VuiParametersState::fdump(
    FILE* outfp, int indent_level) const {
  fprintf(outfp, "vui_parameters {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "aspect_ratio_info_present_flag: %i",
          aspect_ratio_info_present_flag);

  if (aspect_ratio_info_present_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "aspect_ratio_idc: %i", aspect_ratio_idc);

    if (aspect_ratio_idc == AR_EXTENDED_SAR) {
      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "sar_width: %i", sar_width);

      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "sar_height: %i", sar_height);
    }
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "overscan_info_present_flag: %i", overscan_info_present_flag);

  if (overscan_info_present_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "overscan_appropriate_flag: %i", overscan_appropriate_flag);
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "video_signal_type_present_flag: %i",
          video_signal_type_present_flag);

  if (video_signal_type_present_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "video_format: %i", video_format);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "video_full_range_flag: %i", video_full_range_flag);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "colour_description_present_flag: %i",
            colour_description_present_flag);

    if (colour_description_present_flag) {
      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "colour_primaries: %i", colour_primaries);

      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "transfer_characteristics: %i", transfer_characteristics);

      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "matrix_coefficients: %i", matrix_coefficients);
    }
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "chroma_loc_info_present_flag: %i",
          chroma_loc_info_present_flag);

  if (chroma_loc_info_present_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "chroma_sample_loc_type_top_field: %i",
            chroma_sample_loc_type_top_field);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "chroma_sample_loc_type_bottom_field: %i",
            chroma_sample_loc_type_bottom_field);
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "timing_info_present_flag: %i", timing_info_present_flag);

  if (timing_info_present_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "num_units_in_tick: %i", num_units_in_tick);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "time_scale: %i", time_scale);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "fixed_frame_rate_flag: %i", fixed_frame_rate_flag);
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "nal_hrd_parameters_present_flag: %i",
          nal_hrd_parameters_present_flag);

  if (nal_hrd_parameters_present_flag) {
    fdump_indent_level(outfp, indent_level);
    nal_hrd_parameters->fdump(outfp, indent_level);
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "vcl_hrd_parameters_present_flag: %i",
          vcl_hrd_parameters_present_flag);

  if (vcl_hrd_parameters_present_flag) {
    fdump_indent_level(outfp, indent_level);
    vcl_hrd_parameters->fdump(outfp, indent_level);
  }

  if (nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "low_delay_hrd_flag: %i", low_delay_hrd_flag);
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "pic_struct_present_flag: %i", pic_struct_present_flag);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "bitstream_restriction_flag: %i", bitstream_restriction_flag);

  if (bitstream_restriction_flag) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "motion_vectors_over_pic_boundaries_flag: %i",
            motion_vectors_over_pic_boundaries_flag);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "max_bytes_per_pic_denom: %i", max_bytes_per_pic_denom);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "max_bits_per_mb_denom: %i", max_bits_per_mb_denom);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "log2_max_mv_length_horizontal: %i",
            log2_max_mv_length_horizontal);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "log2_max_mv_length_vertical: %i",
            log2_max_mv_length_vertical);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "max_num_reorder_frames: %i", max_num_reorder_frames);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "max_dec_frame_buffering: %i", max_dec_frame_buffering);
  }

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}
#endif  // FDUMP_DEFINE

}  // namespace h264nal
