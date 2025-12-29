# 1. CLAUDE.md - h264nal Codebase Reference

> AI-friendly documentation for the h264nal project
> Last updated: 2025-12-29

# 1.1. Project Overview

h264nal is a production-grade C++ library and CLI tool for parsing H.264/AVC (Advanced Video Coding) NAL (Network Abstraction Layer) units. It accepts H.264 Annex B format files and produces human-readable dumps of parsed NAL unit contents.

* Version: 0.25
* License: BSD
* Copyright: Facebook, Inc. and its affiliates
* Repository: https://github.com/chemag/h264nal
* Companion Project: h265nal (for H.265/HEVC parsing)

# 1.2. Quick Start for AI Assistants

# 1.2.1. Common Tasks

1. Finding a parser: All parsers are in `/include/h264_*_parser.h` with implementations in `/src/h264_*_parser.cc`
2. Running tests: `cd build && make test` (16+ unit tests)
3. Building: `mkdir build && cd build && cmake .. && make`
4. Main entry point: `/tools/h264nal.cc` for CLI tool
5. Bitstream parsing: Start with `/include/h264_bitstream_parser.h`

# 1.2.2. Three Integration Levels

```cpp
// Level 1: Full Bitstream (Annex B format)
H264BitstreamParser::ParseBitstream()

// Level 2: Single NAL Unit
H264NalUnitParser::ParseNalUnit()

// Level 3: RTP Packets (RFC6184)
H264RtpParser::ParseRtp()
```

# 1.3. Architecture

# 1.3.1. Parser Pattern

Every parser follows this consistent pattern:

```cpp
class H264[Component]Parser {
 public:
  // State structure (immutable - deleted copy/move constructors)
  struct [Component]State {
    [Component]State() = default;
    [Component]State(const [Component]State&) = delete;
    [Component]State& operator=(const [Component]State&) = delete;
    // ... fields ...
  };

  // Static parsing method
  static std::unique_ptr<[Component]State> Parse[Component](
      rtc::BitBuffer* bit_buffer, ...);

  // Optional formatted dump (if FDUMP_DEFINE)
  static void fdump(const [Component]State& state, FILE* outfp);
};
```

# 1.3.2. Directory Structure

```
h264nal/
|-- include/          # 28 public header files
|-- src/              # 27 implementation files (.cc)
|-- tools/            # CLI tool (h264nal.cc)
|-- test/             # 23 unit test files
|-- fuzz/             # 18 auto-generated fuzzers + corpus
|-- media/            # Test video files and documentation
|-- ya_getopt/        # Windows getopt port
+-- build/            # CMake generated files
```

# 1.3.3. Key Components by Category

# 1.3.3.1. Core NAL Unit Parsing

* `h264_nal_unit_parser.h` - Master NAL unit parser
* `h264_nal_unit_header_parser.h` - NAL unit headers
* `h264_nal_unit_payload_parser.h` - Payload dispatcher

# 1.3.3.2. Parameter Sets

* `h264_sps_parser.h` - Sequence Parameter Set (SPS)
* `h264_pps_parser.h` - Picture Parameter Set (PPS)
* `h264_subset_sps_parser.h` - Subset SPS (scalable coding)
* `h264_sps_extension_parser.h` - SPS extensions
* `h264_sps_svc_extension_parser.h` - SVC SPS extensions

# 1.3.3.3. Slice/Frame Structures

* `h264_slice_header_parser.h` - Slice headers
* `h264_slice_layer_without_partitioning_rbsp_parser.h` - Slice data layer
* `h264_slice_layer_extension_rbsp_parser.h` - Slice extensions
* `h264_slice_header_in_scalable_extension_parser.h` - SVC slice headers

# 1.3.3.4. RTP Support (RFC6184)

* `h264_rtp_parser.h` - RTP dispatcher
* `h264_rtp_single_parser.h` - Single NAL unit RTP packets
* `h264_rtp_stapa_parser.h` - STAP-A (Single-Time Aggregation)
* `h264_rtp_fua_parser.h` - FU-A (Fragmentation Units)

# 1.3.3.5. Auxiliary Structures

* `h264_hrd_parameters_parser.h` - Hypothetical Reference Decoder params
* `h264_vui_parameters_parser.h` - Video Usability Information
* `h264_ref_pic_list_modification_parser.h` - Reference picture lists
* `h264_pred_weight_table_parser.h` - Weighted prediction
* `h264_dec_ref_pic_marking_parser.h` - Reference picture marking
* `h264_prefix_nal_unit_parser.h` - Prefix NAL units

# 1.3.3.6. Utilities

* `h264_common.h` - Enums, constants, type definitions
* `h264_utils.h` - Utility functions
* `rtc_common.h` - Bit buffer operations (from WebRTC)
* `h264_bitstream_parser_state.h` - Persistent SPS/PPS state

# 1.4. Build System

# 1.4.1. CMake Configuration

* Build System: CMake 3.10+
* C++ Standard: C++14 (Linux/Mac), C++20 (Windows)
* Main Files: Root `CMakeLists.txt` plus subdirectory configs

# 1.4.2. Compiler Flags (Debug)

```bash
-g -O0 -Wall -Wextra -Wunused-parameter -Wshadow -Wformat
-Wextra-semi -Wsign-conversion -Werror
-Wextra-semi-stmt (Clang only)
```

# 1.4.3. Compile-Time Configuration

```cpp
FDUMP_DEFINE      // Enable fdump() diagnostic output
RTP_DEFINE        // Enable RTP parsing
FPRINT_ERRORS     // Enable error printing
SMALL_FOOTPRINT   // Minimal feature build
```

# 1.4.4. Dependencies

* gtest-devel - Unit testing
* gmock-devel - Google Mock
* llvm-toolset/clang - Fuzzing support
* WebRTC - Bit buffer (integrated, no external dep)

# 1.4.5. Build Commands

```bash
# Standard build
mkdir build && cd build
cmake ..
make
make test

# With options
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
cmake -DBUILD_H264_TESTS=OFF ..
cmake -DBUILD_CLANG_FUZZER=OFF ..
```

# 1.5. Testing Infrastructure

# 1.5.1. Unit Tests (23 test files)

All tests in `/test/` directory using Google Test framework.

Key Test Files:

* `h264_bitstream_parser_unittest.cc` - End-to-end bitstream tests
* `h264_sps_parser_unittest.cc` - SPS parsing
* `h264_pps_parser_unittest.cc` - PPS parsing
* `h264_slice_header_parser_unittest.cc` - Slice headers
* `h264_rtp_*_parser_unittest.cc` - RTP variants
* And 18 more component-specific tests

Running Tests:

```bash
cd build
make test                    # All tests
./test/h264_sps_parser_test  # Individual test
```

# 1.5.2. Fuzzing (18 fuzzers)

* Framework: libfuzzer with ASAN/UBSan
* Generation: Auto-generated from unit tests via `/fuzz/converter.py`
* Corpus: Available in `fuzz/corpus/` submodule
* Markers: Code tagged with `// fuzzer::conv: begin/end`

Building Fuzzers:

```bash
cmake -DBUILD_CLANG_FUZZER=ON ..
make
./fuzz/h264_sps_parser_fuzzer
```

# 1.6. CLI Tool Usage

Executable: `/tools/h264nal`

# 1.6.1. Basic Usage

```bash
# Simple parse
./h264nal file.264

# Formatted output with metadata
./h264nal file.264 --noas-one-line --add-offset --add-length --add-parsed-length

# Get resolution
./h264nal file.264 --noisy-parsing

# Explicit NAL length framing
./h264nal file.264 --nalu-length-size 4
```

# 1.6.2. Output Options

* `--noas-one-line` - Multi-line formatted output
* `--add-offset` - Show byte offset
* `--add-length` - Show NAL unit length
* `--add-parsed-length` - Show parsed bytes
* `--add-checksum` - Add CRC32 checksum
* `--noisy-parsing` - Debug output

# 1.7. Code Patterns & Conventions

# 1.7.1. State Management

Immutable States:

```cpp
struct SpsDataState {
  SpsDataState() = default;
  SpsDataState(const SpsDataState&) = delete;  // No copy
  SpsDataState& operator=(const SpsDataState&) = delete;
  // Fields...
};
```

Persistent State:

```cpp
// Maintains SPS/PPS across NAL units
H264BitstreamParserState state;
H264BitstreamParser::ParseBitstream(data, len, &state);
```

# 1.7.2. Namespace

All code in `h264nal` namespace (flat, no nesting).

# 1.7.3. Bit-Level Parsing

```cpp
// WebRTC BitBuffer for bit-precise reading
rtc::BitBuffer bit_buffer(data, length);

// Exponential Golomb codes
bit_buffer.ReadExponentialGolomb(&value);  // ue(v)
bit_buffer.ReadSignedExponentialGolomb(&value);  // se(v)

// RBSP de-escaping handled automatically
```

# 1.7.4. Error Handling

* Returns `nullptr` on parse errors
* Optional error printing with `FPRINT_ERRORS`
* Validation against H.264 spec constraints

# 1.8. Standards References

The code implements:

* ITU-T H.264 (08/2021)
* ISO/IEC 14496-10 (AVC specification)
* RFC 6184 (RTP Payload Format for H.264)

Comments reference specific sections, e.g., "// 7.3.2.1.1"

# 1.9. Known Limitations

From README Section 8:

* No STAP-B, MTAP16, MTAP24, or FU-B RTP modes
* SEI (Supplemental Enhancement Information) parser not implemented
* Some auxiliary parsers missing (see TODO)

# 1.10. Recent Changes (v0.25)

From git history and `status.md`:

* Fixed -Wextra-semi-stmt warnings (Clang)
* Resolved 242 format specifier warnings
* Fixed 138 + 10 extra semicolon warnings
* Sign conversion warnings addressed
* Centralized compiler flags in CMakeLists

# 1.11. Common Maintenance Tasks

# 1.11.1. Adding a New Parser

1. Create header in `/include/h264_new_parser.h`
2. Implement in `/src/h264_new_parser.cc`
3. Add unit test in `/test/h264_new_parser_unittest.cc`
4. Add fuzzer markers to test
5. Regenerate fuzzers: `cd fuzz && ./converter.py`
6. Update CMakeLists.txt files

# 1.11.2. Fixing Compiler Warnings

1. Check current warnings: `make 2>&1 | tee warnings.txt`
2. Update CMakeLists.txt compiler flags
3. Fix code systematically by component
4. Verify tests still pass
5. Document in `status.md` or `warn.md`

# 1.11.3. Running Specific Tests

```bash
cd build

# Single parser test
./test/h264_sps_parser_test

# With verbose output
./test/h264_sps_parser_test --gtest_verbose

# Specific test case
./test/h264_sps_parser_test --gtest_filter=H264SpsParserTest.TestSample
```

# 1.12. File Naming Conventions

* Headers: `h264_component_parser.h`
* Sources: `h264_component_parser.cc`
* Tests: `h264_component_parser_unittest.cc`
* Fuzzers: `h264_component_parser_fuzzer.cc`

All files use snake_case.

# 1.13. Portability

# 1.13.1. Platform Support

* Linux: Primary development platform
* macOS: Fully supported
* Windows: Supported with ya_getopt library

# 1.13.2. Platform-Specific Code

* Getopt: Conditional `ya_getopt` on Windows
* Sockets: `winsock2.h` on Windows
* Designated initializers: C++20 required on Windows

# 1.14. Quality Indicators

* Comprehensive test coverage (23 unit tests)
* Fuzzing infrastructure for security
* Strict compiler warnings (`-Werror`)
* Production use (Facebook copyright)
* Active maintenance (recent v0.25)
* Clean API with multiple integration levels
* Standards-compliant implementation

# 1.15. Useful Commands

```bash
# Full build and test
make clean && cmake .. && make && make test

# Check for warnings
make 2>&1 | grep -i warning

# Run fuzzer with corpus
./fuzz/h264_sps_parser_fuzzer fuzz/corpus/h264_sps_parser_fuzzer/

# Format check (if using clang-format)
find src include -name "*.cc" -o -name "*.h" | xargs clang-format -i

# Parse test file
./tools/h264nal media/test.264 --noas-one-line
```

# 1.16. Documentation Files

* `README.md` - Main project documentation
* `media/README.md` - Test video documentation
* `fuzz/README.md` - Fuzzing infrastructure
* `status.md` - Recent build improvements
* `warn.md` - Warning analysis (23,081 warnings)
* `LICENSE` - BSD license

# 1.17. Contact & Resources

* Repository: https://github.com/chemag/h264nal
* Issues: File on GitHub
* Related: h265nal for HEVC/H.265

---

*This document is designed to help AI assistants and developers quickly understand the h264nal codebase structure, patterns, and common tasks.*
