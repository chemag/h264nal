/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_bitstream_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h264_bitstream_parser_state.h"
#include "h264_common.h"
#include "rtc_base/arraysize.h"
#include "rtc_base/bit_buffer.h"

namespace h264nal {

class H264BitstreamParserTest : public ::testing::Test {
 public:
  H264BitstreamParserTest() {}
  ~H264BitstreamParserTest() override {}
};

// SPS, PPS (601.264)
// fuzzer::conv: data
const uint8_t buffer[] = {
    // SPS
    0x00, 0x00, 0x00, 0x01, 0x67, 0x42, 0xc0, 0x16,
    0xa6, 0x11, 0x05, 0x07, 0xe9, 0xb2, 0x00, 0x00,
    0x03, 0x00, 0x02, 0x00, 0x00, 0x03, 0x00, 0x64,
    0x1e, 0x2c, 0x5c, 0x23,
    // PPS
    0x00, 0x00, 0x00, 0x01, 0x68, 0xc8, 0x42, 0x02,
    0x32, 0xc8
};

TEST_F(H264BitstreamParserTest, TestSampleBitstream601) {
  // SPS/PPS (601.264)
  // fuzzer::conv: begin
  // init the BitstreamParserState
  H264BitstreamParserState bitstream_parser_state;

  auto bitstream = H264BitstreamParser::ParseBitstream(
      buffer, arraysize(buffer), &bitstream_parser_state);
  // fuzzer::conv: end

  EXPECT_TRUE(bitstream != nullptr);

  // check there are 2 NAL units
  EXPECT_EQ(2, bitstream->nal_units.size());

  // check the 1st NAL unit
  int index = 0;
  EXPECT_EQ(0,
            bitstream->nal_units[index]->nal_unit_header->forbidden_zero_bit);
  EXPECT_EQ(3, bitstream->nal_units[index]->nal_unit_header->nal_ref_idc);
  EXPECT_EQ(NalUnitType::SPS_NUT,
            bitstream->nal_units[index]->nal_unit_header->nal_unit_type);

  // check the 2nd NAL unit
  index += 1;
  EXPECT_EQ(0,
            bitstream->nal_units[index]->nal_unit_header->forbidden_zero_bit);
  EXPECT_EQ(3, bitstream->nal_units[index]->nal_unit_header->nal_ref_idc);
  EXPECT_EQ(NalUnitType::PPS_NUT,
            bitstream->nal_units[index]->nal_unit_header->nal_unit_type);
}

TEST_F(H264BitstreamParserTest, TestSampleBitstream601Alt) {
  // init the BitstreamParserState
  bool add_offset = true;
  bool add_length = true;
  bool add_parsed_length = true;
  auto bitstream = H264BitstreamParser::ParseBitstream(
      buffer, arraysize(buffer), add_offset, add_length, add_parsed_length);
  EXPECT_TRUE(bitstream != nullptr);

  // check there are 2 NAL units
  EXPECT_EQ(2, bitstream->nal_units.size());

  // check the NAL unit locations
  int length = 0;
  int counter = 0;
  // 1st NAL unit
  int index = 0;
  length = 24;
  counter += 4;
  EXPECT_EQ(counter, bitstream->nal_units[index]->offset);
  EXPECT_EQ(length, bitstream->nal_units[index]->length);
  EXPECT_EQ(22, bitstream->nal_units[index]->parsed_length);
  index += 1;
  counter += length;
  // 2nd NAL unit
  length = 6;
  counter += 4;
  EXPECT_EQ(counter, bitstream->nal_units[index]->offset);
  EXPECT_EQ(length, bitstream->nal_units[index]->length);
  EXPECT_EQ(6, bitstream->nal_units[index]->parsed_length);
  //index += 1;
  //counter += length;
}

}  // namespace h264nal
