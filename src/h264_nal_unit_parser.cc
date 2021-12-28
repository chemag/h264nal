/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_nal_unit_parser.h"

#include <stdio.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "h264_bitstream_parser_state.h"
#include "h264_common.h"
#include "h264_pps_parser.h"
#include "h264_slice_layer_without_partitioning_rbsp_parser.h"
#include "h264_sps_parser.h"

namespace h264nal {

// General note: this is based off the 2012 version of the H.264 standard.
// You can find it on this page:
// http://www.itu.int/rec/T-REC-H.264

// Unpack RBSP and parse NAL Unit state from the supplied buffer.
std::unique_ptr<H264NalUnitParser::NalUnitState>
H264NalUnitParser::ParseNalUnit(
    const uint8_t* data, size_t length,
    struct H264BitstreamParserState* bitstream_parser_state,
    bool add_checksum) noexcept {
  std::vector<uint8_t> unpacked_buffer = UnescapeRbsp(data, length);
  rtc::BitBuffer bit_buffer(unpacked_buffer.data(), unpacked_buffer.size());

  return ParseNalUnit(&bit_buffer, bitstream_parser_state, add_checksum);
}

std::unique_ptr<H264NalUnitParser::NalUnitState>
H264NalUnitParser::ParseNalUnit(
    rtc::BitBuffer* bit_buffer,
    struct H264BitstreamParserState* bitstream_parser_state,
    bool add_checksum) noexcept {
  // H264 NAL Unit (nal_unit()) parser.
  // Section 7.3.1 ("NAL unit syntax") of the H.264
  // standard for a complete description.
  auto nal_unit = std::make_unique<NalUnitState>();

  // need to calculate the checksum before parsing the bit buffer
  if (add_checksum) {
    // set the checksum
    nal_unit->checksum = NaluChecksum::GetNaluChecksum(bit_buffer);
  }

  // nal_unit_header()
  nal_unit->nal_unit_header =
      H264NalUnitHeaderParser::ParseNalUnitHeader(bit_buffer);
  if (nal_unit->nal_unit_header == nullptr) {
#ifdef FPRINT_ERRORS
    fprintf(stderr, "error: cannot ParseNalUnitHeader in nal unit\n");
#endif  // FPRINT_ERRORS
    return nullptr;
  }

  // nal_unit_payload()
  nal_unit->nal_unit_payload = H264NalUnitPayloadParser::ParseNalUnitPayload(
      bit_buffer, nal_unit->nal_unit_header->nal_ref_idc,
      nal_unit->nal_unit_header->nal_unit_type, bitstream_parser_state);

  // update the parsed length
  nal_unit->parsed_length = get_current_offset(bit_buffer);

  return nal_unit;
}

// Unpack RBSP and parse NAL Unit header state from the supplied buffer.
std::unique_ptr<H264NalUnitHeaderParser::NalUnitHeaderState>
H264NalUnitHeaderParser::ParseNalUnitHeader(const uint8_t* data,
                                            size_t length) noexcept {
  std::vector<uint8_t> unpacked_buffer = UnescapeRbsp(data, length);
  rtc::BitBuffer bit_buffer(unpacked_buffer.data(), unpacked_buffer.size());

  return ParseNalUnitHeader(&bit_buffer);
}

bool H264NalUnitHeaderParser::GetNalUnitType(const uint8_t* data,
                                             const size_t length,
                                             NalUnitType& naluType) noexcept {
  rtc::BitBuffer bitBuffer(data, length);
  auto naluHeader = ParseNalUnitHeader(&bitBuffer);
  if (!naluHeader) {
    return false;
  }
  naluType = static_cast<NalUnitType>(naluHeader->nal_unit_type);
  return true;
}

std::unique_ptr<H264NalUnitHeaderParser::NalUnitHeaderState>
H264NalUnitHeaderParser::ParseNalUnitHeader(
    rtc::BitBuffer* bit_buffer) noexcept {
  // H264 NAL Unit Header (nal_unit_header()) parser.
  // Section 7.3.1.2 ("NAL unit header syntax") of the H.264
  // standard for a complete description.
  auto nal_unit_header = std::make_unique<NalUnitHeaderState>();

  // forbidden_zero_bit  f(1)
  if (!bit_buffer->ReadBits(&nal_unit_header->forbidden_zero_bit, 1)) {
    return nullptr;
  }

  // nal_ref_idc  u(2)
  if (!bit_buffer->ReadBits(&nal_unit_header->nal_ref_idc, 2)) {
    return nullptr;
  }

  // nal_unit_type  u(5)
  if (!bit_buffer->ReadBits(&nal_unit_header->nal_unit_type, 5)) {
    return nullptr;
  }

  if (nal_unit_header->nal_unit_type == 14 ||
      nal_unit_header->nal_unit_type == 20 ||
      nal_unit_header->nal_unit_type == 21) {
    if (nal_unit_header->nal_unit_type != 21) {
      // svc_extension_flag  u(1)
      if (!bit_buffer->ReadBits(&nal_unit_header->svc_extension_flag, 1)) {
        return nullptr;
      }

    } else {
      // avc_3d_extension_flag  u(1)
      if (!bit_buffer->ReadBits(&nal_unit_header->avc_3d_extension_flag, 1)) {
        return nullptr;
      }
    }
  }

  return nal_unit_header;
}

// Unpack RBSP and parse NAL Unit payload state from the supplied buffer.
std::unique_ptr<H264NalUnitPayloadParser::NalUnitPayloadState>
H264NalUnitPayloadParser::ParseNalUnitPayload(
    const uint8_t* data, size_t length, uint32_t nal_ref_idc,
    uint32_t nal_unit_type,
    struct H264BitstreamParserState* bitstream_parser_state) noexcept {
  std::vector<uint8_t> unpacked_buffer = UnescapeRbsp(data, length);
  rtc::BitBuffer bit_buffer(unpacked_buffer.data(), unpacked_buffer.size());

  return ParseNalUnitPayload(&bit_buffer, nal_ref_idc, nal_unit_type,
                             bitstream_parser_state);
}

std::unique_ptr<H264NalUnitPayloadParser::NalUnitPayloadState>
H264NalUnitPayloadParser::ParseNalUnitPayload(
    rtc::BitBuffer* bit_buffer, uint32_t nal_ref_idc, uint32_t nal_unit_type,
    struct H264BitstreamParserState* bitstream_parser_state) noexcept {
  // H264 NAL Unit Payload (nal_unit()) parser.
  // Section 7.3.1 ("NAL unit syntax") of the H.264
  // standard for a complete description.
  auto nal_unit_payload = std::make_unique<NalUnitPayloadState>();

  // payload (Table 7-1, Section 7.4.1)
  switch (nal_unit_type) {
    case CODED_SLICE_OF_NON_IDR_PICTURE_NUT: {
      // slice_layer_without_partitioning_rbsp()
      nal_unit_payload->slice_layer_without_partitioning_rbsp =
          H264SliceLayerWithoutPartitioningRbspParser::
              ParseSliceLayerWithoutPartitioningRbsp(bit_buffer, nal_ref_idc,
                                                     nal_unit_type,
                                                     bitstream_parser_state);
      break;
    }
    case CODED_SLICE_DATA_PARTITION_A_NUT:
    case CODED_SLICE_DATA_PARTITION_B_NUT:
    case CODED_SLICE_DATA_PARTITION_C_NUT:
      // unimplemented
      break;
    case CODED_SLICE_OF_IDR_PICTURE_NUT: {
      // slice_layer_without_partitioning_rbsp()
      nal_unit_payload->slice_layer_without_partitioning_rbsp =
          H264SliceLayerWithoutPartitioningRbspParser::
              ParseSliceLayerWithoutPartitioningRbsp(bit_buffer, nal_ref_idc,
                                                     nal_unit_type,
                                                     bitstream_parser_state);
      break;
    }
    case SEI_NUT:
      // unimplemented
      break;
    case SPS_NUT: {
      // seq_parameter_set_rbsp()
      nal_unit_payload->sps = H264SpsParser::ParseSps(bit_buffer);
      if (nal_unit_payload->sps != nullptr) {
        uint32_t sps_id = nal_unit_payload->sps->sps_data->seq_parameter_set_id;
        bitstream_parser_state->sps[sps_id] = nal_unit_payload->sps;
      }
      break;
    }
    case PPS_NUT: {
      // pic_parameter_set_rbsp()
      nal_unit_payload->pps = H264PpsParser::ParsePps(bit_buffer);
      if (nal_unit_payload->pps != nullptr) {
        uint32_t pps_id = nal_unit_payload->pps->pic_parameter_set_rbsp;
        bitstream_parser_state->pps[pps_id] = nal_unit_payload->pps;
      }
      break;
    }

    case AUD_NUT:
    case EOSEQ_NUT:
    case EOSTREAM_NUT:
    case FILLER_DATA_NUT:
    case SPS_EXTENSION_NUT:
    case PREFIX_NUT:
    case SUBSET_SPS_NUT:
      // unimplemented
      break;
    case RSV16_NUT:
    case RSV17_NUT:
    case RSV18_NUT:
      // reserved
      break;
    case CODED_SLICE_OF_AUXILIARY_CODED_PICTURE_NUT:
    case CODED_SLICE_EXTENSION:
      // unimplemented
      break;
    case RSV21_NUT:
    case RSV22_NUT:
    case RSV23_NUT:
      // reserved
      break;
    case UNSPECIFIED_NUT:
    case UNSPEC24_NUT:
    case UNSPEC25_NUT:
    case UNSPEC26_NUT:
    case UNSPEC27_NUT:
    case UNSPEC28_NUT:
    case UNSPEC29_NUT:
    case UNSPEC30_NUT:
    case UNSPEC31_NUT:
    default:
      // unspecified
      break;
  }

  return nal_unit_payload;
}

#ifdef FDUMP_DEFINE
void H264NalUnitParser::NalUnitState::fdump(FILE* outfp, int indent_level,
                                            bool add_offset, bool add_length,
                                            bool add_parsed_length,
                                            bool add_checksum) const {
  fprintf(outfp, "nal_unit {");
  indent_level = indent_level_incr(indent_level);

  // nal unit offset (starting at NAL unit header)
  if (add_offset) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "offset: 0x%08zx", offset);
  }

  // nal unit length (starting at NAL unit header)
  if (add_length) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "length: %zu", length);
  }

  // nal unit parsed length (starting at NAL unit header)
  if (add_parsed_length) {
    fdump_indent_level(outfp, indent_level);
    fprintf(outfp, "parsed_length: 0x%08zx", parsed_length);
  }

  // nal unit checksum
  if (add_checksum) {
    fdump_indent_level(outfp, indent_level);
    char checksum_printable[64] = {};
    checksum->fdump(checksum_printable, 64);
    fprintf(outfp, "checksum: 0x%s", checksum_printable);
  }

  // header
  fdump_indent_level(outfp, indent_level);
  nal_unit_header->fdump(outfp, indent_level);

  // payload
  fdump_indent_level(outfp, indent_level);
  nal_unit_payload->fdump(outfp, indent_level, nal_unit_header->nal_unit_type);

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}
#endif  // FDUMP_DEFINE

#ifdef FDUMP_DEFINE
void H264NalUnitHeaderParser::NalUnitHeaderState::fdump(
    FILE* outfp, int indent_level) const {
  fprintf(outfp, "nal_unit_header {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "forbidden_zero_bit: %i", forbidden_zero_bit);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "nal_ref_idc: %i", nal_ref_idc);

  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "nal_unit_type: %i", nal_unit_type);

  if (nal_unit_type == 14 || nal_unit_type == 20 || nal_unit_type == 21) {
    if (nal_unit_type != 21) {
      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "svc_extension_flag: %i", svc_extension_flag);
    } else {
      fdump_indent_level(outfp, indent_level);
      fprintf(outfp, "avc_3d_extension_flag: %i", avc_3d_extension_flag);
    }
  }

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}
#endif  // FDUMP_DEFINE

#ifdef FDUMP_DEFINE
void H264NalUnitPayloadParser::NalUnitPayloadState::fdump(
    FILE* outfp, int indent_level, uint32_t nal_unit_type) const {
  fprintf(outfp, "nal_unit_payload {");
  indent_level = indent_level_incr(indent_level);

  fdump_indent_level(outfp, indent_level);
  switch (nal_unit_type) {
    case CODED_SLICE_OF_NON_IDR_PICTURE_NUT:
      slice_layer_without_partitioning_rbsp->fdump(outfp, indent_level);
      break;
    case CODED_SLICE_DATA_PARTITION_A_NUT:
    case CODED_SLICE_DATA_PARTITION_B_NUT:
    case CODED_SLICE_DATA_PARTITION_C_NUT:
      // unimplemented
      break;
    case CODED_SLICE_OF_IDR_PICTURE_NUT:
      slice_layer_without_partitioning_rbsp->fdump(outfp, indent_level);
      break;
    case SEI_NUT:
      // unimplemented
      break;
    case SPS_NUT:
      if (sps) {
        sps->fdump(outfp, indent_level);
      }
      break;
    case PPS_NUT:
      if (pps) {
        pps->fdump(outfp, indent_level);
      }
      break;
    case AUD_NUT:
    case EOSEQ_NUT:
    case EOSTREAM_NUT:
    case FILLER_DATA_NUT:
    case SPS_EXTENSION_NUT:
    case PREFIX_NUT:
    case SUBSET_SPS_NUT:
      // unimplemented
      break;
    case RSV16_NUT:
    case RSV17_NUT:
    case RSV18_NUT:
      // reserved
      break;
    case CODED_SLICE_OF_AUXILIARY_CODED_PICTURE_NUT:
    case CODED_SLICE_EXTENSION:
      // unimplemented
      break;
    case RSV21_NUT:
    case RSV22_NUT:
    case RSV23_NUT:
      // reserved
      break;
    case UNSPECIFIED_NUT:
    case UNSPEC24_NUT:
    case UNSPEC25_NUT:
    case UNSPEC26_NUT:
    case UNSPEC27_NUT:
    case UNSPEC28_NUT:
    case UNSPEC29_NUT:
    case UNSPEC30_NUT:
    case UNSPEC31_NUT:
    default:
      // unspecified
      break;
  }

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}
#endif  // FDUMP_DEFINE

}  // namespace h264nal
