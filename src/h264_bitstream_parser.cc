/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_bitstream_parser.h"

#if defined WIN32 || defined _WIN32 || defined __CYGWIN__
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif
#include <stdio.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "h264_bitstream_parser_state.h"
#include "h264_common.h"
#include "h264_nal_unit_parser.h"

namespace {
// The size of a full NALU start sequence {0 0 0 1}, used for the first NALU
// of an access unit, and for SPS, PPS, and SubsetSPS blocks.
// const size_t kNaluLongStartSequenceSize = 4;
// The size of a shortened NALU start sequence {0 0 1}, that may be used if
// not the first NALU of an access unit or an SPS, PPS, or SubsetSPS block.
const size_t kNaluShortStartSequenceSize = 3;

size_t read_nalu_length(const uint8_t* data, size_t nalu_length_bytes) {
  size_t nalu_length = 0;
  for (size_t i = 0; i < nalu_length_bytes; ++i) {
    nalu_length = (nalu_length << 8) + data[i];
  }
  return nalu_length;
}
}  // namespace

namespace h264nal {

// General note: this is based off the 2012 version of the H.264 standard.
// You can find it on this page:
// http://www.itu.int/rec/T-REC-H.264

std::vector<H264BitstreamParser::NaluIndex>
H264BitstreamParser::FindNaluIndices(const uint8_t* data,
                                     size_t length) noexcept {
  // This is sorta like Boyer-Moore, but with only the first optimization step:
  // given a 3-byte sequence we're looking at, if the 3rd byte isn't 1 or 0,
  // skip ahead to the next 3-byte sequence. 0s and 1s are relatively rare, so
  // this will skip the majority of reads/checks.
  std::vector<NaluIndex> sequences;
  if (length < kNaluShortStartSequenceSize) {
    return sequences;
  }

  const size_t end = length - kNaluShortStartSequenceSize;
  for (size_t i = 0; i < end;) {
    if (data[i + 2] > 1) {
      i += 3;
    } else if (data[i + 2] == 0x01 && data[i + 1] == 0x00 && data[i] == 0x00) {
      // We found a start sequence, now check if it was a 3 of 4 byte one.
      NaluIndex index = {i, i + 3, 0};
      if (index.start_offset > 0 && data[index.start_offset - 1] == 0)
        --index.start_offset;

      // Update length of previous entry.
      auto it = sequences.rbegin();
      if (it != sequences.rend())
        it->payload_size = index.start_offset - it->payload_start_offset;

      sequences.push_back(index);

      i += 3;
    } else {
      ++i;
    }
  }

  // Update length of last entry, if any.
  auto it = sequences.rbegin();
  if (it != sequences.rend())
    it->payload_size = length - it->payload_start_offset;

  return sequences;
}

// NALU search for buffers with explicit nal unit size fields
std::vector<H264BitstreamParser::NaluIndex>
H264BitstreamParser::FindNaluIndicesExplicitFraming(const uint8_t* data,
                                                    size_t length) noexcept {
  // Assumes 4-byte nal_unit sizes.
  std::vector<NaluIndex> sequences;
  const size_t end = length;
  for (size_t i = 0; i < end;) {
    // read a nal_unit_size (4 bytes)
    uint32_t nal_unit_size = ntohl(*(uint32_t*)(data + i));
    // This is a start sequence
    NaluIndex index = {i, i + 4, nal_unit_size};
    sequences.push_back(index);
    i += (4 + nal_unit_size);
  }

  return sequences;
}

// Parse a raw (RBSP) buffer with explicit NAL unit separator (3- or 4-byte
// sequence start code prefix). Function splits the stream in NAL units,
// and then parses each NAL unit. For that, it unpacks the RBSP inside
// each NAL unit buffer, and adds the corresponding parsed struct into
// the `bitstream` list (a `BitstreamState` object).
// Function returns the said `bitstream` list.
std::unique_ptr<H264BitstreamParser::BitstreamState>
H264BitstreamParser::ParseBitstream(
    const uint8_t* data, size_t length,
    H264BitstreamParserState* bitstream_parser_state,
    ParsingOptions parsing_options) noexcept {
  auto bitstream = std::make_unique<BitstreamState>();

  // (1) split the input string into a vector of NAL units
  std::vector<NaluIndex> nalu_indices = FindNaluIndices(data, length);
  if (nalu_indices.size() == 0) {
#ifdef FPRINT_ERRORS
    fprintf(stderr,
            "error: no NAL units found on bitstream (zero NALU indices)\n");
#endif  // FPRINT_ERRORS
  }

  // process each of the NAL units
  for (const NaluIndex& nalu_index : nalu_indices) {
    // (2) parse the NAL units, and add them to the vector
    auto nal_unit = H264NalUnitParser::ParseNalUnit(
        &data[nalu_index.payload_start_offset], nalu_index.payload_size,
        bitstream_parser_state, parsing_options);
    if (nal_unit == nullptr) {
      // cannot parse the NalUnit
#ifdef FPRINT_ERRORS
      fprintf(stderr, "error: cannot parse buffer into NalUnit\n");
#endif  // FPRINT_ERRORS
      continue;
    }
    // store the offset
    nal_unit->offset = nalu_index.payload_start_offset;
    nal_unit->length = nalu_index.payload_size;

    bitstream->nal_units.push_back(std::move(nal_unit));
  }

  return bitstream;
}

std::unique_ptr<H264BitstreamParser::BitstreamState>
H264BitstreamParser::ParseBitstream(const uint8_t* data, size_t length,
                                    ParsingOptions parsing_options) noexcept {
  // keep a bitstream parser state (to keep the SPS/PPS/SubsetSPS NALUs)
  H264BitstreamParserState bitstream_parser_state;

  // parse the file
  auto bitstream =
      ParseBitstream(data, length, &bitstream_parser_state, parsing_options);
  if (bitstream == nullptr) {
    // did not work
#ifdef FPRINT_ERRORS
    fprintf(stderr, "Could not init h264 bitstream parser\n");
#endif  // FPRINT_ERRORS
    return nullptr;
  }
  bitstream->parsing_options_ = parsing_options;
  return bitstream;
}

// Parse a raw (RBSP) buffer with explicit NAL unit length fields.
// Function splits the stream in NAL units, and then parses each NAL unit.
// For that, it unpacks the RBSP inside each NAL unit buffer, and adds the
// corresponding parsed struct into the `bitstream` list (a `BitstreamState`
// object).
// Function returns the said `bitstream` list.
std::unique_ptr<H264BitstreamParser::BitstreamState>
H264BitstreamParser::ParseBitstreamNALULength(
    const uint8_t* data, size_t length, size_t nalu_length_bytes,
    H264BitstreamParserState* bitstream_parser_state,
    ParsingOptions parsing_options) noexcept {
  auto bitstream = std::make_unique<BitstreamState>();

  size_t i = 0;

  if (length < nalu_length_bytes) {
#ifdef FPRINT_ERRORS
    fprintf(stderr,
            "error: got %zu bytes, less than the NALU length size (%zu)\n",
            length, nalu_length_bytes);
#endif  // FPRINT_ERRORS
  }

  // process each of the NAL units
  for (i = 0; i < length;) {
    size_t nalu_length = 0;
    if (nalu_length_bytes > 0) {
      // (1) read the NALU length
      nalu_length = read_nalu_length(data, nalu_length_bytes);
      i += nalu_length_bytes;
    } else {
      // assume a single NALU
      nalu_length = length;
    }

    // (2) parse the NAL unit, and add it to the vector
    auto nal_unit = H264NalUnitParser::ParseNalUnit(
        &data[i], nalu_length, bitstream_parser_state, parsing_options);
    if (nal_unit == nullptr) {
      // cannot parse the NalUnit
#ifdef FPRINT_ERRORS
      fprintf(stderr, "error: cannot parse buffer into NalUnit\n");
#endif  // FPRINT_ERRORS
      continue;
    }
    // store the offset
    nal_unit->offset = i;
    nal_unit->length = nalu_length_bytes;

    bitstream->nal_units.push_back(std::move(nal_unit));

    i += nalu_length;
  }

  return bitstream;
}

std::unique_ptr<H264BitstreamParser::BitstreamState>
H264BitstreamParser::ParseBitstreamNALULength(
    const uint8_t* data, size_t length, size_t nalu_length_bytes,

    ParsingOptions parsing_options) noexcept {
  // keep a bitstream parser state (to keep the VPS/PPS/SPS NALUs)
  H264BitstreamParserState bitstream_parser_state;

  // parse the file
  auto bitstream =
      ParseBitstreamNALULength(data, length, nalu_length_bytes,
                               &bitstream_parser_state, parsing_options);
  if (bitstream == nullptr) {
    // did not work
#ifdef FPRINT_ERRORS
    fprintf(stderr, "Could not init h264 bitstream parser\n");
#endif  // FPRINT_ERRORS
    return nullptr;
  }
  bitstream->parsing_options_ = parsing_options;
  return bitstream;
}

#ifdef FDUMP_DEFINE
void H264BitstreamParser::BitstreamState::fdump(
    FILE* outfp, int indent_level, ParsingOptions parsing_options) const {
  for (auto& nal_unit : nal_units) {
    nal_unit->fdump(outfp, indent_level, parsing_options_);
    fprintf(outfp, "\n");
  }
}
#endif  // FDUMP_DEFINE

}  // namespace h264nal
