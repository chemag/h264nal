/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_sps_extension_parser.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>

#include <cmath>
#include <cstdint>
#include <memory>
#include <vector>

#include "h264_common.h"
#include "h264_vui_parameters_parser.h"

namespace {
typedef std::shared_ptr<h264nal::H264SpsExtensionParser::SpsExtensionState>
    SharedPtrSpsExtension;
}  // namespace

namespace h264nal {

// General note: this is based off the 2012 version of the H.264 standard.
// You can find it on this page:
// http://www.itu.int/rec/T-REC-H.264

// Unpack RBSP and parse SPS state from the supplied buffer.
std::shared_ptr<H264SpsExtensionParser::SpsExtensionState>
H264SpsExtensionParser::ParseSpsExtension(const uint8_t* data,
                                          size_t length) noexcept {
  std::vector<uint8_t> unpacked_buffer = UnescapeRbsp(data, length);
  rtc::BitBuffer bit_buffer(unpacked_buffer.data(), unpacked_buffer.size());
  return ParseSpsExtension(&bit_buffer);
}

std::shared_ptr<H264SpsExtensionParser::SpsExtensionState>
H264SpsExtensionParser::ParseSpsExtension(rtc::BitBuffer* bit_buffer) noexcept {
  // H264 SPS Nal Unit (seq_parameter_set_extension_rbsp()) parser.
  // Section 7.3.2.1.2 ("Sequence parameter set extension syntax") of the
  // H.264 standard for a complete description.
  auto sps_extension = std::make_shared<SpsExtensionState>();

  // seq_parameter_set_id  ue(v)
  if (!bit_buffer->ReadExponentialGolomb(sps_extension->seq_parameter_set_id)) {
    return nullptr;
  }
  if (sps_extension->seq_parameter_set_id < kSeqParameterSetIdMin ||
      sps_extension->seq_parameter_set_id > kSeqParameterSetIdMax) {
#ifdef FPRINT_ERRORS
    fprintf(stderr,
            "invalid seq_parameter_set_id: %" PRIu32
            " not in range "
            "[%" PRIu32 ", %" PRIu32 "]\n",
            sps_extension->seq_parameter_set_id, kSeqParameterSetIdMin,
            kSeqParameterSetIdMax);
#endif  // FPRINT_ERRORS
    return nullptr;
  }

  // aux_format_idc  ue(v)
  if (!bit_buffer->ReadExponentialGolomb(sps_extension->aux_format_idc)) {
    return nullptr;
  }

  if (sps_extension->aux_format_idc != 0) {
    // bit_depth_aux_minus8  ue(v)
    if (!bit_buffer->ReadExponentialGolomb(
            sps_extension->bit_depth_aux_minus8)) {
      return nullptr;
    }
    if (sps_extension->bit_depth_aux_minus8 < kBitDepthAuxMinus8Min ||
        sps_extension->bit_depth_aux_minus8 > kBitDepthAuxMinus8Max) {
#ifdef FPRINT_ERRORS
      fprintf(stderr,
              "invalid bit_depth_aux_minus8: %" PRIu32
              " not in range "
              "[%" PRIu32 ", %" PRIu32 "]\n",
              sps_extension->bit_depth_aux_minus8, kBitDepthAuxMinus8Min,
              kBitDepthAuxMinus8Max);
#endif  // FPRINT_ERRORS
      return nullptr;
    }

    // alpha_incr_flag  u(1)
    if (!bit_buffer->ReadBits(1, sps_extension->alpha_incr_flag)) {
      return nullptr;
    }

    // alpha_opaque_value  u(v)
    int alpha_opaque_value_bits = sps_extension->bit_depth_aux_minus8 + 9;
    if (!bit_buffer->ReadBits(alpha_opaque_value_bits,
                              sps_extension->alpha_opaque_value)) {
      return nullptr;
    }

    // alpha_transparent_value  u(v)
    int alpha_transparent_value_bits = sps_extension->bit_depth_aux_minus8 + 9;
    if (!bit_buffer->ReadBits(alpha_transparent_value_bits,
                              sps_extension->alpha_transparent_value)) {
      return nullptr;
    }
  }

  // additional_extension_flag  u(1)
  if (!bit_buffer->ReadBits(1, sps_extension->additional_extension_flag)) {
    return nullptr;
  }

  rbsp_trailing_bits(bit_buffer);

  return sps_extension;
}

#ifdef FDUMP_DEFINE
void H264SpsExtensionParser::SpsExtensionState::fdump(FILE* outfp,
                                                      int indent_level) const {
  fprintf(outfp, "sps_extension {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "seq_parameter_set_id: %i", seq_parameter_set_id);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "aux_format_idc: %i", aux_format_idc);

  if (aux_format_idc != 0) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "bit_depth_aux_minus8: %i", bit_depth_aux_minus8);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "alpha_incr_flag: %i", alpha_incr_flag);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "alpha_opaque_value: %i", alpha_opaque_value);

    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "alpha_transparent_value: %i", alpha_transparent_value);
  }

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "additional_extension_flag: %i", additional_extension_flag);

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}
#endif  // FDUMP_DEFINE

}  // namespace h264nal
