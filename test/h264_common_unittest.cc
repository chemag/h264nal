/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_common.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

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

}  // namespace h264nal
