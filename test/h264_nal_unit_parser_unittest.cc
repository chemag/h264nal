/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_nal_unit_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h264_bitstream_parser_state.h"
#include "h264_common.h"
#include "rtc_base/arraysize.h"
#include "rtc_base/bit_buffer.h"

namespace h264nal {

class H264NalUnitParserTest : public ::testing::Test {
 public:
  H264NalUnitParserTest() {}
  ~H264NalUnitParserTest() override {}
};

TEST_F(H264NalUnitParserTest, TestSampleNalUnit) {
  // fuzzer::conv: data
  const uint8_t buffer[] = {0x67};
  // fuzzer::conv: begin
  H264BitstreamParserState bitstream_parser_state;
  auto nal_unit = H264NalUnitParser::ParseNalUnit(buffer, arraysize(buffer),
                                                  &bitstream_parser_state);
  // fuzzer::conv: end

  EXPECT_TRUE(nal_unit != nullptr);

  // check the parsed length
  EXPECT_EQ(1, nal_unit->parsed_length);

  // check the header
  EXPECT_EQ(0, nal_unit->nal_unit_header->forbidden_zero_bit);
  EXPECT_EQ(3, nal_unit->nal_unit_header->nal_ref_idc);
  EXPECT_EQ(NalUnitType::SPS_NUT, nal_unit->nal_unit_header->nal_unit_type);
}

TEST_F(H264NalUnitParserTest, TestEmptyNalUnit) {
  const uint8_t buffer[] = {};
  H264BitstreamParserState bitstream_parser_state;
  auto nal_unit = H264NalUnitParser::ParseNalUnit(buffer, 0,
                                                  &bitstream_parser_state);
  EXPECT_TRUE(nal_unit == nullptr);
}

class H264NalUnitHeaderParserTest : public ::testing::Test {
 public:
  H264NalUnitHeaderParserTest() {}
  ~H264NalUnitHeaderParserTest() override {}
};

}  // namespace h264nal
