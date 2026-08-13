/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_nal_unit_parser.h"

#include "h264_nal_unit_header_parser.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "h264_bitstream_parser_state.h"
#include "h264_common.h"
#include "rtc_common.h"

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
  ParsingOptions parsing_options;
  parsing_options.add_checksum = true;
  auto nal_unit = H264NalUnitParser::ParseNalUnit(buffer, arraysize(buffer),
                                                  &bitstream_parser_state,
                                                  parsing_options);
  // fuzzer::conv: end

  EXPECT_TRUE(nal_unit != nullptr);

  // check the parsed length
  EXPECT_EQ(1, nal_unit->parsed_length);

  // check the checksum
  EXPECT_THAT(std::vector<char>(nal_unit->checksum->GetChecksum(),
                                nal_unit->checksum->GetChecksum() +
                                    nal_unit->checksum->GetLength()),
              ::testing::ElementsAreArray({0x98, 0xff, 0xff, 0xff}));

  // test the checksum's ascii dumper
  char checksum_printable[64] = {0};
  nal_unit->checksum->fdump(checksum_printable, 64);
  EXPECT_STREQ(checksum_printable, "98ffffff");

  // check the header
  EXPECT_EQ(0, nal_unit->nal_unit_header->forbidden_zero_bit);
  EXPECT_EQ(3, nal_unit->nal_unit_header->nal_ref_idc);
  EXPECT_EQ(NalUnitType::SPS_NUT, nal_unit->nal_unit_header->nal_unit_type);

  // a 1-byte SPS NAL unit has a header but no parseable payload. The NAL
  // unit survives, so this is the only way a caller notices; the h264nal
  // tool uses it to pick its exit code.
  ASSERT_TRUE(nal_unit->nal_unit_payload != nullptr);
  EXPECT_FALSE(nal_unit->nal_unit_payload->IsPayloadParsed(
      nal_unit->nal_unit_header->nal_unit_type));
  // a type whose payload we do not parse at all is not missing anything
  EXPECT_TRUE(
      nal_unit->nal_unit_payload->IsPayloadParsed(NalUnitType::SEI_NUT));
}

TEST_F(H264NalUnitParserTest, TestEmptyNalUnit) {
  const uint8_t buffer[] = {0};
  H264BitstreamParserState bitstream_parser_state;
  ParsingOptions parsing_options;
  parsing_options.add_checksum = false;
  auto nal_unit =
      H264NalUnitParser::ParseNalUnit(buffer, 0, &bitstream_parser_state,
                                      parsing_options);
  EXPECT_TRUE(nal_unit == nullptr);
}

TEST_F(H264NalUnitParserTest, TestTruncatedNalUnit) {
  // 1-byte NAL unit: header only, no payload to parse
  const uint8_t buffer[] = {0x67};
  H264BitstreamParserState bitstream_parser_state;
  ParsingOptions parsing_options;
  parsing_options.add_checksum = false;
  auto nal_unit =
      H264NalUnitParser::ParseNalUnit(buffer, sizeof(buffer),
                                      &bitstream_parser_state,
                                      parsing_options);
  // 1-byte NAL unit has a valid header but the payload parse may fail
  // depending on the NAL unit type; either outcome is acceptable
  // as long as it does not crash
}

class H264NalUnitHeaderParserTest : public ::testing::Test {
 public:
  H264NalUnitHeaderParserTest() {}
  ~H264NalUnitHeaderParserTest() override {}
};

// Section 7.3.1: for nal_unit_type 14, 20 and 21 the header is followed by
// exactly one of three extension structures. Only the SVC one is parsed;
// the other two are refused rather than skipped over, because their bytes
// would otherwise be read as the start of the payload.

TEST_F(H264NalUnitHeaderParserTest, TestSvcExtensionHeader) {
  // nal_unit_type 20 with svc_extension_flag 1 (svcbcts-1.264)
  const uint8_t buffer[] = {0x74, 0xc0, 0x10, 0x07, 0xb4};
  auto nal_unit_header =
      H264NalUnitHeaderParser::ParseNalUnitHeader(buffer, arraysize(buffer));

  ASSERT_TRUE(nal_unit_header != nullptr);
  EXPECT_EQ(0, nal_unit_header->forbidden_zero_bit);
  EXPECT_EQ(3, nal_unit_header->nal_ref_idc);
  EXPECT_EQ(NalUnitType::CODED_SLICE_EXTENSION,
            nal_unit_header->nal_unit_type);
  EXPECT_EQ(1, nal_unit_header->svc_extension_flag);
  EXPECT_TRUE(nal_unit_header->nal_unit_header_svc_extension != nullptr);
}

TEST_F(H264NalUnitHeaderParserTest, TestMvcExtensionHeaderIsRefused) {
  // nal_unit_type 20 with svc_extension_flag 0 (mvcds1.264), so the header
  // carries a nal_unit_header_mvc_extension(). Parsing on without consuming
  // its 3 bytes used to shift the slice header by 24 bits and report the
  // resulting garbage as a bad bitstream, e.g. "invalid pic_parameter_set_id:
  // 3148 not in range [0, 255]".
  const uint8_t buffer[] = {0x74, 0x02, 0x00, 0x45, 0xb4};
  auto nal_unit_header =
      H264NalUnitHeaderParser::ParseNalUnitHeader(buffer, arraysize(buffer));

  EXPECT_TRUE(nal_unit_header == nullptr);
}

TEST_F(H264NalUnitHeaderParserTest, Test3davcExtensionHeaderIsRefused) {
  // nal_unit_type 21 with avc_3d_extension_flag 1 (balloons.bit), so the
  // header carries a nal_unit_header_3davc_extension() of 2 bytes.
  const uint8_t buffer[] = {0x55, 0x81, 0xc3, 0xb5, 0x5f};
  auto nal_unit_header =
      H264NalUnitHeaderParser::ParseNalUnitHeader(buffer, arraysize(buffer));

  EXPECT_TRUE(nal_unit_header == nullptr);
}

}  // namespace h264nal
