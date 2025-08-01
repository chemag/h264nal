/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_slice_layer_extension_rbsp_parser.h"


#include <cinttypes>
#include <stdio.h>

#include <cmath>
#include <cstdint>
#include <memory>
#include <vector>

#include "h264_bitstream_parser_state.h"
#include "h264_common.h"
#include "h264_nal_unit_header_parser.h"
#include "h264_slice_header_parser.h"
#include "rtc_common.h"

namespace h264nal {

// General note: this is based off the 2016/02 version of the H.264 standard.
// You can find it on this page:
// http://www.itu.int/rec/T-REC-H.264

// Unpack RBSP and parse slice header state from the supplied buffer.
std::unique_ptr<H264SliceLayerExtensionRbspParser::SliceLayerExtensionRbspState>
H264SliceLayerExtensionRbspParser::ParseSliceLayerExtensionRbsp(
    const uint8_t* data, size_t length,
    H264NalUnitHeaderParser::NalUnitHeaderState& nal_unit_header,
    struct H264BitstreamParserState* bitstream_parser_state) noexcept {
  std::vector<uint8_t> unpacked_buffer = UnescapeRbsp(data, length);
  BitBuffer bit_buffer(unpacked_buffer.data(), unpacked_buffer.size());
  return ParseSliceLayerExtensionRbsp(&bit_buffer, nal_unit_header,
                                      bitstream_parser_state);
}

std::unique_ptr<H264SliceLayerExtensionRbspParser::SliceLayerExtensionRbspState>
H264SliceLayerExtensionRbspParser::ParseSliceLayerExtensionRbsp(
    BitBuffer* bit_buffer,
    H264NalUnitHeaderParser::NalUnitHeaderState& nal_unit_header,
    struct H264BitstreamParserState* bitstream_parser_state) noexcept {
  // H264 slice (slice_layer_extension_rbsp()) NAL Unit.
  // Section 7.3.2.13 ("Slice layer extension RBSP syntax") of
  // the H.264 standard for a complete description.
  auto slice_layer_extension_rbsp =
      std::make_unique<SliceLayerExtensionRbspState>();

  // input parameters
  slice_layer_extension_rbsp->svc_extension_flag =
      nal_unit_header.svc_extension_flag;
  slice_layer_extension_rbsp->avc_3d_extension_flag =
      nal_unit_header.avc_3d_extension_flag;
  slice_layer_extension_rbsp->nal_ref_idc = nal_unit_header.nal_ref_idc;
  slice_layer_extension_rbsp->nal_unit_type = nal_unit_header.nal_unit_type;

  if (slice_layer_extension_rbsp->svc_extension_flag) {
    // slice_header_in_scalable_extension()  // specified in Annex G
    slice_layer_extension_rbsp->slice_header_in_scalable_extension =
        H264SliceHeaderInScalableExtensionParser::
            ParseSliceHeaderInScalableExtension(bit_buffer, nal_unit_header,
                                                bitstream_parser_state);
    // if (!slice_skip_flag) {
    //   slice_data_in_scalable_extension()  // specified in Annex G
    // }

  } else if (slice_layer_extension_rbsp->avc_3d_extension_flag) {
    // slice_header_in_3davc_extension()  // specified in Annex J
    // slice_data_in_3davc_extension()  // specified in Annex J

  } else {
    // slice_header(slice_type)
    slice_layer_extension_rbsp->slice_header =
        H264SliceHeaderParser::ParseSliceHeader(
            bit_buffer, slice_layer_extension_rbsp->nal_ref_idc,
            slice_layer_extension_rbsp->nal_unit_type, bitstream_parser_state);
    if (slice_layer_extension_rbsp->slice_header == nullptr) {
      return nullptr;
    }
    // slice_data()
  }

  // rbsp_slice_trailing_bits()

  return slice_layer_extension_rbsp;
}

#ifdef FDUMP_DEFINE
void H264SliceLayerExtensionRbspParser::SliceLayerExtensionRbspState::fdump(
    FILE* outfp, int indent_level) const {
  fprintf(outfp, "slice_layer_extension_rbsp {");
  indent_level = indent_level_incr(indent_level);

  if (svc_extension_flag) {
    fdump_indent_level(outfp, indent_level);
    if (slice_header_in_scalable_extension) {
      slice_header_in_scalable_extension->fdump(outfp, indent_level);
    }
    // if (!slice_skip_flag) {
    //   slice_data_in_scalable_extension()  // specified in Annex G
    // }

  } else if (avc_3d_extension_flag) {
    // slice_header_in_3davc_extension()  // specified in Annex J
    // slice_data_in_3davc_extension()  // specified in Annex J

  } else {
    fdump_indent_level(outfp, indent_level);
    slice_header->fdump(outfp, indent_level);
    // slice_data()
  }

  // rbsp_slice_trailing_bits()

  indent_level = indent_level_decr(indent_level);
  fdump_indent_level(outfp, indent_level);
  fprintf(outfp, "}");
}
#endif  // FDUMP_DEFINE

}  // namespace h264nal
