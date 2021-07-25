/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_common.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <vector>

#include "rtc_base/arraysize.h"
#include "rtc_base/bit_buffer.h"

namespace h264nal {

class H264CommonTest : public ::testing::Test {
 public:
  H264CommonTest() {}
  ~H264CommonTest() override {}
};

TEST_F(H264CommonTest, TestIsNalUnitTypeReserved) {
  EXPECT_TRUE(IsNalUnitTypeReserved(RSV13_NUT));
  EXPECT_TRUE(IsNalUnitTypeReserved(RSV23_NUT));
  EXPECT_FALSE(IsNalUnitTypeReserved(UNSPEC24_NUT));
}

TEST_F(H264CommonTest, TestIsNalUnitTypeUnspecified) {
  EXPECT_TRUE(IsNalUnitTypeUnspecified(UNSPEC24_NUT));
  EXPECT_TRUE(IsNalUnitTypeUnspecified(UNSPEC31_NUT));
  EXPECT_FALSE(IsNalUnitTypeUnspecified(RSV13_NUT));
}

struct H264CommonMoreRbspDataParameterTestData {
  int line;
  std::vector<uint8_t> buffer;
  size_t cur_byte_offset;
  size_t cur_bit_offset;
  bool expected_result;
  bool expected_get_last_bit_offset_return;
  size_t expected_last_one_byte_offset;
  size_t expected_last_one_bit_offset;
};

class H264CommonMoreRbspDataTest
    : public ::testing::TestWithParam<H264CommonMoreRbspDataParameterTestData> {
};

const auto& kH264CommonMoreRbspDataParameterTestcases = *new std::vector<
    H264CommonMoreRbspDataParameterTestData>{
    {__LINE__, {0xe8, 0x43, 0x82, 0x92, 0xc8, 0xb0}, 4, 4, true, true, 5, 3},
    {__LINE__, {0xe8, 0x43, 0x82, 0x92, 0xc8, 0xb1}, 4, 4, true, true, 5, 7},
    {__LINE__, {0xe8, 0x43, 0x82, 0x92, 0xc8, 0x00}, 4, 4, false, true, 4, 4},
    {__LINE__, {0x00, 0x00}, 0, 0, false, false, 0, 0},
};

TEST_P(H264CommonMoreRbspDataTest, Run) {
  const auto& testcase = GetParam();
  rtc::BitBuffer bit_buffer(testcase.buffer.data(), testcase.buffer.size());
  bit_buffer.Seek(testcase.cur_byte_offset, testcase.cur_bit_offset);
  // first check more_rbsp_data
  EXPECT_EQ(testcase.expected_result, more_rbsp_data(&bit_buffer))
      << "line: " << testcase.line;

  // then check BitBuffer::GetCurrentOffset()
  size_t expected_last_one_byte_offset = 0;
  size_t expected_last_one_bit_offset = 0;
  EXPECT_EQ(testcase.expected_get_last_bit_offset_return,
            bit_buffer.GetLastBitOffset(1, &expected_last_one_byte_offset,
                                        &expected_last_one_bit_offset))
      << "line: " << testcase.line;
  if (testcase.expected_get_last_bit_offset_return) {
    EXPECT_EQ(testcase.expected_last_one_byte_offset,
              expected_last_one_byte_offset)
        << "line: " << testcase.line;
    EXPECT_EQ(testcase.expected_last_one_bit_offset,
              expected_last_one_bit_offset)
        << "line: " << testcase.line;
  }
}

INSTANTIATE_TEST_SUITE_P(
    Parameter, H264CommonMoreRbspDataTest,
    ::testing::ValuesIn(kH264CommonMoreRbspDataParameterTestcases));

}  // namespace h264nal
