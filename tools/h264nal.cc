/*
 *  Copyright (c) Facebook, Inc. and its affiliates.
 *
 * An Annex-B Parser. It reads a full h264 (AVC) Annex-B file, and parses
 * it using a single function (`H264BitstreamParser::ParseBitstream()`).
 * It then dumps the contents of each NALU read.
 */

#if defined WIN32 || defined _WIN32 || defined __CYGWIN__
#include "ya_getopt.h"
#else
#include <getopt.h>
#endif
#include <stdio.h>
#include <stdlib.h>

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "config.h"
#include "h264_bitstream_parser.h"
#include "h264_common.h"
#include "h264_utils.h"
#include "rtc_common.h"

extern int optind;

typedef struct arg_options {
  int debug;
  bool as_one_line;
  bool add_offset;
  bool add_length;
  bool add_parsed_length;
  bool add_checksum;
  bool add_resolution;
  bool add_contents;
  char *infile;
  char *outfile;
} arg_options;

// default option values
arg_options DEFAULT_OPTIONS{
    .debug = 0,
    .as_one_line = true,
    .add_offset = false,
    .add_length = false,
    .add_parsed_length = false,
    .add_checksum = false,
    .add_resolution = false,
    .add_contents = false,
    .infile = nullptr,
    .outfile = nullptr,
};

void usage(char *name) {
  fprintf(stderr, "usage: %s [options]\n", name);
  fprintf(stderr, "where options are:\n");
  fprintf(stderr, "\t-d:\t\tIncrease debug verbosity [default: %i]\n",
          DEFAULT_OPTIONS.debug);
  fprintf(stderr, "\t-q:\t\tZero debug verbosity\n");
  fprintf(stderr, "\t-i <infile>:\t\tH264 file to parse [default: stdin]\n");
  fprintf(stderr, "\t-o <output>:\t\tH264 parsing output [default: stdout]\n");
  fprintf(stderr, "\t--as-one-line:\tSet as_one_line flag%s\n",
          DEFAULT_OPTIONS.as_one_line ? " [default]" : "");
  fprintf(stderr, "\t--no-as-one-line:\tReset as_one_line flag%s\n",
          !DEFAULT_OPTIONS.as_one_line ? " [default]" : "");
  fprintf(stderr, "\t--add-offset:\tSet add_offset flag%s\n",
          DEFAULT_OPTIONS.add_offset ? " [default]" : "");
  fprintf(stderr, "\t--no-add-offset:\tReset add_offset flag%s\n",
          !DEFAULT_OPTIONS.add_offset ? " [default]" : "");
  fprintf(stderr, "\t--add-length:\tSet add_length flag%s\n",
          DEFAULT_OPTIONS.add_length ? " [default]" : "");
  fprintf(stderr, "\t--no-add-length:\tReset add_length flag%s\n",
          !DEFAULT_OPTIONS.add_length ? " [default]" : "");
  fprintf(stderr, "\t--add-parsed-length:\tSet add_parsed_length flag%s\n",
          DEFAULT_OPTIONS.add_parsed_length ? " [default]" : "");
  fprintf(stderr, "\t--no-add-parsed-length:\tReset add_parsed_length flag%s\n",
          !DEFAULT_OPTIONS.add_parsed_length ? " [default]" : "");
  fprintf(stderr, "\t--add-checksum:\tSet add_checksum flag%s\n",
          DEFAULT_OPTIONS.add_checksum ? " [default]" : "");
  fprintf(stderr, "\t--no-add-checksum:\tReset add_checksum flag%s\n",
          !DEFAULT_OPTIONS.add_checksum ? " [default]" : "");
  fprintf(stderr, "\t--add-resolution:\tSet add_resolution flag%s\n",
          DEFAULT_OPTIONS.add_resolution ? " [default]" : "");
  fprintf(stderr, "\t--no-add-resolution:\tReset add_resolution flag%s\n",
          !DEFAULT_OPTIONS.add_resolution ? " [default]" : "");
  fprintf(stderr, "\t--add-contents:\tSet add_contents flag%s\n",
          DEFAULT_OPTIONS.add_contents ? " [default]" : "");
  fprintf(stderr, "\t--no-add-contents:\tReset add_contents flag%s\n",
          !DEFAULT_OPTIONS.add_contents ? " [default]" : "");
  fprintf(stderr, "\t--version:\t\tDump version number\n");
  fprintf(stderr, "\t-h:\t\tHelp\n");
  exit(-1);
}

// long options with no equivalent short option
enum {
  QUIET_OPTION = CHAR_MAX + 1,
  AS_ONE_LINE_FLAG_OPTION,
  NO_AS_ONE_LINE_FLAG_OPTION,
  ADD_OFFSET_FLAG_OPTION,
  NO_ADD_OFFSET_FLAG_OPTION,
  ADD_LENGTH_FLAG_OPTION,
  NO_ADD_LENGTH_FLAG_OPTION,
  ADD_PARSED_LENGTH_FLAG_OPTION,
  NO_ADD_PARSED_LENGTH_FLAG_OPTION,
  ADD_CHECKSUM_FLAG_OPTION,
  NO_ADD_CHECKSUM_FLAG_OPTION,
  ADD_RESOLUTION_FLAG_OPTION,
  NO_ADD_RESOLUTION_FLAG_OPTION,
  ADD_CONTENTS_FLAG_OPTION,
  NO_ADD_CONTENTS_FLAG_OPTION,
  VERSION_OPTION,
  HELP_OPTION
};

arg_options *parse_args(int argc, char **argv) {
  int c;
  static arg_options options;

  // set default options
  options = DEFAULT_OPTIONS;

  // getopt_long stores the option index here
  int optindex = 0;

  // long options
  static struct option longopts[] = {
      // matching options to short options
      {"debug", no_argument, NULL, 'd'},
      {"infile", required_argument, NULL, 'i'},
      {"outfile", required_argument, NULL, 'o'},
      // options without a short option
      {"quiet", no_argument, NULL, QUIET_OPTION},
      {"as-one-line", no_argument, NULL, AS_ONE_LINE_FLAG_OPTION},
      {"no-as-one-line", no_argument, NULL, NO_AS_ONE_LINE_FLAG_OPTION},
      {"add-offset", no_argument, NULL, ADD_OFFSET_FLAG_OPTION},
      {"no-add-offset", no_argument, NULL, NO_ADD_OFFSET_FLAG_OPTION},
      {"add-length", no_argument, NULL, ADD_LENGTH_FLAG_OPTION},
      {"no-add-length", no_argument, NULL, NO_ADD_LENGTH_FLAG_OPTION},
      {"add-parsed-length", no_argument, NULL, ADD_PARSED_LENGTH_FLAG_OPTION},
      {"no-add-parsed-length", no_argument, NULL,
       NO_ADD_PARSED_LENGTH_FLAG_OPTION},
      {"add-checksum", no_argument, NULL, ADD_CHECKSUM_FLAG_OPTION},
      {"no-add-checksum", no_argument, NULL, NO_ADD_CHECKSUM_FLAG_OPTION},
      {"add-resolution", no_argument, NULL, ADD_RESOLUTION_FLAG_OPTION},
      {"no-add-resolution", no_argument, NULL, NO_ADD_RESOLUTION_FLAG_OPTION},
      {"add-contents", no_argument, NULL, ADD_CONTENTS_FLAG_OPTION},
      {"no-add-contents", no_argument, NULL, NO_ADD_CONTENTS_FLAG_OPTION},
      {"version", no_argument, NULL, VERSION_OPTION},
      {"help", no_argument, NULL, HELP_OPTION},
      {NULL, 0, NULL, 0}};

  // parse arguments
  while ((c = getopt_long(argc, argv, "di:o:h", longopts, &optindex)) != -1) {
    switch (c) {
      case 0:
        // long options that define flag
        // if this option set a flag, do nothing else now
        if (longopts[optindex].flag != NULL) {
          break;
        }
        printf("option %s", longopts[optindex].name);
        if (optarg) {
          printf(" with arg %s", optarg);
        }
        break;

      case 'd':
        options.debug += 1;
        break;

      case QUIET_OPTION:
        options.debug = 0;
        break;

      case 'i':
        options.infile = optarg;
        break;

      case 'o':
        options.outfile = optarg;
        break;

      case AS_ONE_LINE_FLAG_OPTION:
        options.as_one_line = true;
        break;

      case NO_AS_ONE_LINE_FLAG_OPTION:
        options.as_one_line = false;
        break;

      case ADD_OFFSET_FLAG_OPTION:
        options.add_offset = true;
        break;

      case NO_ADD_OFFSET_FLAG_OPTION:
        options.add_offset = false;
        break;

      case ADD_LENGTH_FLAG_OPTION:
        options.add_length = true;
        break;

      case NO_ADD_LENGTH_FLAG_OPTION:
        options.add_length = false;
        break;

      case ADD_PARSED_LENGTH_FLAG_OPTION:
        options.add_parsed_length = true;
        break;

      case NO_ADD_PARSED_LENGTH_FLAG_OPTION:
        options.add_parsed_length = false;
        break;

      case ADD_CHECKSUM_FLAG_OPTION:
        options.add_checksum = true;
        break;

      case NO_ADD_CHECKSUM_FLAG_OPTION:
        options.add_checksum = false;
        break;

      case ADD_RESOLUTION_FLAG_OPTION:
        options.add_resolution = true;
        break;

      case NO_ADD_RESOLUTION_FLAG_OPTION:
        options.add_resolution = false;
        break;

      case ADD_CONTENTS_FLAG_OPTION:
        options.add_contents = true;
        break;

      case NO_ADD_CONTENTS_FLAG_OPTION:
        options.add_contents = false;
        break;

      case VERSION_OPTION:
        printf("version: %s\n", PROJECT_VER);
        exit(0);
        break;

      case HELP_OPTION:
      case 'h':
        usage(argv[0]);
        break;

      default:
        printf("Unsupported option: %c\n", c);
        usage(argv[0]);
    }
  }

  return &options;
}

int main(int argc, char **argv) {
  arg_options *options;

  // parse args
  options = parse_args(argc, argv);
  if (options == nullptr) {
    usage(argv[0]);
    exit(-1);
  }

  if (options->debug > 1) {
#ifdef SMALL_FOOTPRINT
    printf("h264nal: small footprint version\n");
#else
    printf("h264nal: original version\n");
#endif
  }

  // print args
  if (options->debug > 1) {
    printf("options->debug = %i\n", options->debug);
    printf("options->infile = %s\n",
           (options->infile == NULL) ? "null" : options->infile);
    printf("options->outfile = %s\n",
           (options->outfile == NULL) ? "null" : options->outfile);
  }

  // add_contents requires add_length and add_offset
  if (options->add_contents) {
    options->add_offset = true;
    options->add_length = true;
  }

  // 1. read infile into buffer
  std::vector<uint8_t> buffer;
  if (h264nal::H264Utils::ReadFile(options->infile, buffer) < 0) {
    return -1;
  }

  // 2. prepare bitstream parsing
  h264nal::ParsingOptions parsing_options;
  parsing_options.add_offset = options->add_offset;
  parsing_options.add_length = options->add_length;
  parsing_options.add_parsed_length = options->add_parsed_length;
  parsing_options.add_checksum = options->add_checksum;
  parsing_options.add_resolution = options->add_resolution;

  // 3. parse bitstream
  std::unique_ptr<h264nal::H264BitstreamParser::BitstreamState> bitstream =
      h264nal::H264BitstreamParser::ParseBitstream(buffer.data(), buffer.size(),
                                                   parsing_options);

#ifdef FDUMP_DEFINE
  // get outfile file descriptor
  FILE *outfp;
  if (options->outfile == nullptr ||
      (strlen(options->outfile) == 1 && options->outfile[0] == '-')) {
    // use stdout
    outfp = stdout;
  } else {
    outfp = fopen(options->outfile, "wb");
    if (outfp == nullptr) {
      // did not work
      fprintf(stderr, "Could not open output file: \"%s\"\n", options->outfile);
      return -1;
    }
  }

  int indent_level = (options->as_one_line) ? -1 : 0;
  // 4. dump the contents of each NALU
  for (auto &nal_unit : bitstream->nal_units) {
    nal_unit->fdump(outfp, indent_level, parsing_options);
    if (options->add_contents) {
      fprintf(outfp, " contents {");
      for (size_t i = 0; i < nal_unit->length; i++) {
        fprintf(outfp, " %02x", buffer[nal_unit->offset + i]);
        if ((i + 1) % 16 == 0) {
          fprintf(outfp, " ");
        }
      }
      fprintf(outfp, " }");
    }
    fprintf(outfp, "\n");
  }
#endif  // FDUMP_DEFINE

  return 0;
}
