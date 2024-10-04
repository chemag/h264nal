/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_utils.h"

#include <stdio.h>

#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

namespace h264nal {

int H264Utils::ReadFile(const char* filename, std::vector<uint8_t>& buffer) {
  // TODO(chemag): read the infile incrementally
  FILE* infp = nullptr;
  if ((filename == nullptr) || (strlen(filename) == 1 && filename[0] == '-')) {
    infp = stdin;
  } else {
    infp = fopen(filename, "rb");
  }
  if (infp == nullptr) {
    // did not work
    fprintf(stderr, "Could not open input file: \"%s\"\n", filename);
    return -1;
  }
  fseek(infp, 0, SEEK_END);
  int64_t size = ftell(infp);
  fseek(infp, 0, SEEK_SET);
  // read file into buffer
  buffer.resize(size);
  fread(reinterpret_cast<char*>(buffer.data()), 1, size, infp);
  return 0;
}

}  // namespace h264nal
