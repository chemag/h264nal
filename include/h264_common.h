/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#pragma once

#include <stdio.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rtc_common.h"

namespace h264nal {

// Table 7-1 of the 2012 standard.
enum NalUnitType : uint8_t {
  UNSPECIFIED_NUT = 0,
  CODED_SLICE_OF_NON_IDR_PICTURE_NUT = 1,
  CODED_SLICE_DATA_PARTITION_A_NUT = 2,
  CODED_SLICE_DATA_PARTITION_B_NUT = 3,
  CODED_SLICE_DATA_PARTITION_C_NUT = 4,
  CODED_SLICE_OF_IDR_PICTURE_NUT = 5,
  SEI_NUT = 6,
  SPS_NUT = 7,
  PPS_NUT = 8,
  AUD_NUT = 9,
  EOSEQ_NUT = 10,
  EOSTREAM_NUT = 11,
  FILLER_DATA_NUT = 12,
  SPS_EXTENSION_NUT = 13,
  PREFIX_NUT = 14,
  SUBSET_SPS_NUT = 15,
  // 16-18: reserved
  RSV16_NUT = 16,
  RSV17_NUT = 17,
  RSV18_NUT = 18,
  CODED_SLICE_OF_AUXILIARY_CODED_PICTURE_NUT = 19,
  CODED_SLICE_EXTENSION = 20,
  // 21-23: reserved
  RSV21_NUT = 21,
  RSV22_NUT = 22,
  RSV23_NUT = 23,
  // 24-29: RTP
  RTP_STAPA_NUT = 24,
  RTP_FUA_NUT = 28,
  // 24-31: unspecified
  UNSPEC24_NUT = 24,
  UNSPEC25_NUT = 25,
  UNSPEC26_NUT = 26,
  UNSPEC27_NUT = 27,
  UNSPEC28_NUT = 28,
  UNSPEC29_NUT = 29,
  UNSPEC30_NUT = 30,
  UNSPEC31_NUT = 31,
};

bool IsNalUnitTypeReserved(uint32_t nal_unit_type);
bool IsNalUnitTypeUnspecified(uint32_t nal_unit_type);

// P_ALL is defined in <sys/wait.h> in POSIX systems
#ifdef P_ALL
#undef P_ALL
#endif

// Table 7-3
enum SliceType : uint8_t {
  P = 0,
  B = 1,
  I = 2,
  SP = 3,
  SI = 4,
  // slice_type values in the range 5..9 specify, in addition to the coding
  // type of the current slice, that all other slices of the current coded
  // picture shall have a value of slice_type equal to the current value of
  // slice_type or equal to the current value of slice_type - 5.
  P_ALL = 5,
  B_ALL = 6,
  I_ALL = 7,
  SP_ALL = 8,
  SI_ALL = 9,
};

// Section A.2 Profiles
enum ProfileType : uint8_t {
  UNSPECIFIED = 0,
  BASELINE = 1,
  CONSTRAINED_BASELINE = 2,
  MAIN = 3,
  EXTENDED = 4,
  HIGH = 5,
  PROGRESSIVE_HIGH = 6,
  CONSTRAINED_HIGH = 7,
  HIGH_10 = 8,
  PROGRESSIVE_HIGH_10 = 9,
  HIGH_10_INTRA = 10,
  HIGH_422 = 11,
  HIGH_422_INTRA = 12,
  HIGH_444 = 13,
  HIGH_444_INTRA = 14,
  CAVLC_444_INTRA = 15,
};

enum ProfileType getProfileType();
void profileTypeToString(enum ProfileType profile, std::string &str);

// Table G-1
enum SvcSliceType : uint8_t {
  EPa = 0,
  EBa = 1,
  EIa = 2,
  EPb = 5,
  EBb = 6,
  EIb = 7,
};

// Methods for parsing RBSP. See section 7.4 of the H264 spec.
//
// Decoding is simply a matter of finding any 00 00 03 sequence and removing
// the 03 byte (emulation byte).

// Remove any emulation byte escaping from a buffer. This is needed for
// byte-stream format packetization (e.g. Annex B data), but not for
// packet-stream format packetization (e.g. RTP payloads).
std::vector<uint8_t> UnescapeRbsp(const uint8_t *data, size_t length);

// Syntax functions and descriptors) (Section 7.2)
bool byte_aligned(BitBuffer *bit_buffer);
int get_current_offset(BitBuffer *bit_buffer);
bool more_rbsp_data(BitBuffer *bit_buffer);
bool rbsp_trailing_bits(BitBuffer *bit_buffer);

#if defined(FDUMP_DEFINE)
// fdump() indentation help
int indent_level_incr(int indent_level);
int indent_level_decr(int indent_level);
void fdump_indent_level(FILE *outfp, int indent_level);
#endif  // FDUMP_DEFINE

// Generic Parsing Options
struct ParsingOptions {
  bool add_offset;
  bool add_length;
  bool add_parsed_length;
  bool add_checksum;
  bool add_resolution;
  ParsingOptions()
      : add_offset(true),
        add_length(true),
        add_parsed_length(true),
        add_checksum(true),
        add_resolution(true) {}
};

class NaluChecksum {
 public:
  // maximum length (in bytes)
  const static int kMaxLength = 32;
  static std::shared_ptr<NaluChecksum> GetNaluChecksum(
      BitBuffer *bit_buffer) noexcept;
  void fdump(char *output, int output_len) const;
  const char *GetChecksum() { return checksum; };
  int GetLength() { return length; };
  const char *GetPrintableChecksum() const;

 private:
  char checksum[kMaxLength];
  int length;
};

// some ffmpeg constants
const uint32_t kMaxMbWidth = 1055;
const uint32_t kMaxMbHeight = 1055;
const uint32_t kMaxWidth = (kMaxMbWidth * 16);
const uint32_t kMaxHeight = (kMaxMbHeight * 16);
const uint32_t kMaxMbPicSize = 139264;

}  // namespace h264nal
