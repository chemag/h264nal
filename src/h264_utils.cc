/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 */

#include "h264_utils.h"

#include <stdio.h>

#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <vector>

namespace {
int ReadStdinToBuffer(std::vector<uint8_t>& buffer) {
  constexpr size_t chunk_size = 4096;
  uint8_t temp[chunk_size];
  while (true) {
    std::cin.read(reinterpret_cast<char*>(temp), chunk_size);
    std::streamsize bytes_read = std::cin.gcount();
    if (bytes_read <= 0) break;
    buffer.insert(buffer.end(), temp, temp + bytes_read);
  }
  return 0;
}
}  // namespace

namespace h264nal {

int H264Utils::ReadFile(const char* filename, std::vector<uint8_t>& buffer) {
  // TODO(chemag): read the infile incrementally
  FILE* infp = nullptr;
  if ((filename == nullptr) || (strlen(filename) == 1 && filename[0] == '-')) {
    // read from stdin
    return ReadStdinToBuffer(buffer);
  }

  // open the file
  infp = fopen(filename, "rb");
  if (infp == nullptr) {
    // did not work
    fprintf(stderr, "Could not open input file: \"%s\"\n", filename);
    return -1;
  }
  fseek(infp, 0, SEEK_END);
  int64_t size = ftell(infp);
  if (size <= 0) {
    fprintf(stderr, "Could not determine file size: \"%s\"\n", filename);
    fclose(infp);
    return -1;
  }
  fseek(infp, 0, SEEK_SET);
  // read file into buffer
  buffer.resize(static_cast<size_t>(size));
  size_t bytes_read = fread(reinterpret_cast<char*>(buffer.data()), 1,
                            static_cast<size_t>(size), infp);
  // clean up
  fclose(infp);
  if (bytes_read != static_cast<size_t>(size)) {
    fprintf(stderr,
            "Could not read entire file: \"%s\" "
            "(read %zu of %zu bytes)\n",
            filename, bytes_read, static_cast<size_t>(size));
    return -1;
  }
  return 0;
}

}  // namespace h264nal
