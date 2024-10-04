/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#pragma once

#include <stdio.h>

#include <memory>
#include <vector>

namespace h264nal {

// A class for unclassified utilities.
class H264Utils {
 public:
  static int ReadFile(const char* filename, std::vector<uint8_t>& buffer);
};

}  // namespace h264nal
