# The recipes below pipe into tee, and a pipeline reports the status of
# its last command, so without pipefail a crashing fuzzer would look like
# a success. pipefail is not in POSIX sh, hence bash. This applies to
# every recipe in this file.
SHELL := /bin/bash
.SHELLFLAGS := -o pipefail -c





all: \
    h264_common_fuzzer.cc \
    h264_sps_parser_fuzzer.cc \
    h264_hrd_parameters_parser_fuzzer.cc \
    h264_vui_parameters_parser_fuzzer.cc \
    h264_pps_parser_fuzzer.cc \
    h264_ref_pic_list_modification_parser_fuzzer.cc \
    h264_pred_weight_table_parser_fuzzer.cc \
    h264_dec_ref_pic_marking_parser_fuzzer.cc \
    h264_rtp_single_parser_fuzzer.cc \
    h264_rtp_parser_fuzzer.cc \
    h264_rtp_stapa_parser_fuzzer.cc \
    h264_rtp_fua_parser_fuzzer.cc \
    h264_slice_header_parser_fuzzer.cc \
    h264_slice_layer_without_partitioning_rbsp_parser_fuzzer.cc \
    h264_sps_extension_parser_fuzzer.cc \
    h264_subset_sps_parser_fuzzer.cc \
    h264_sps_svc_extension_parser_fuzzer.cc \
    h264_bitstream_parser_fuzzer.cc \
    h264_prefix_nal_unit_parser_fuzzer.cc \
    h264_nal_unit_header_svc_extension_parser_fuzzer.cc \
    h264_nal_unit_parser_fuzzer.cc
	echo "run: RUNS=1000000 make -j -Oline fuzz"


h264_common_fuzzer.cc: ../test/h264_common_unittest.cc
	./converter.py ../test/h264_common_unittest.cc ./

h264_sps_parser_fuzzer.cc: ../test/h264_sps_parser_unittest.cc
	./converter.py ../test/h264_sps_parser_unittest.cc ./

h264_hrd_parameters_parser_fuzzer.cc: ../test/h264_hrd_parameters_parser_unittest.cc
	./converter.py ../test/h264_hrd_parameters_parser_unittest.cc ./

h264_vui_parameters_parser_fuzzer.cc: ../test/h264_vui_parameters_parser_unittest.cc
	./converter.py ../test/h264_vui_parameters_parser_unittest.cc ./

h264_pps_parser_fuzzer.cc: ../test/h264_pps_parser_unittest.cc
	./converter.py ../test/h264_pps_parser_unittest.cc ./

h264_ref_pic_list_modification_parser_fuzzer.cc: ../test/h264_ref_pic_list_modification_parser_unittest.cc
	./converter.py ../test/h264_ref_pic_list_modification_parser_unittest.cc ./

h264_pred_weight_table_parser_fuzzer.cc: ../test/h264_pred_weight_table_parser_unittest.cc
	./converter.py ../test/h264_pred_weight_table_parser_unittest.cc ./

h264_dec_ref_pic_marking_parser_fuzzer.cc: ../test/h264_dec_ref_pic_marking_parser_unittest.cc
	./converter.py ../test/h264_dec_ref_pic_marking_parser_unittest.cc ./

h264_rtp_single_parser_fuzzer.cc: ../test/h264_rtp_single_parser_unittest.cc
	./converter.py ../test/h264_rtp_single_parser_unittest.cc ./

h264_rtp_parser_fuzzer.cc: ../test/h264_rtp_parser_unittest.cc
	./converter.py ../test/h264_rtp_parser_unittest.cc ./

h264_rtp_stapa_parser_fuzzer.cc: ../test/h264_rtp_stapa_parser_unittest.cc
	./converter.py ../test/h264_rtp_stapa_parser_unittest.cc ./

h264_rtp_fua_parser_fuzzer.cc: ../test/h264_rtp_fua_parser_unittest.cc
	./converter.py ../test/h264_rtp_fua_parser_unittest.cc ./

h264_slice_header_parser_fuzzer.cc: ../test/h264_slice_header_parser_unittest.cc
	./converter.py ../test/h264_slice_header_parser_unittest.cc ./

h264_slice_layer_without_partitioning_rbsp_parser_fuzzer.cc: ../test/h264_slice_layer_without_partitioning_rbsp_parser_unittest.cc
	./converter.py ../test/h264_slice_layer_without_partitioning_rbsp_parser_unittest.cc ./

h264_sps_extension_parser_fuzzer.cc: ../test/h264_sps_extension_parser_unittest.cc
	./converter.py ../test/h264_sps_extension_parser_unittest.cc ./

h264_subset_sps_parser_fuzzer.cc: ../test/h264_subset_sps_parser_unittest.cc
	./converter.py ../test/h264_subset_sps_parser_unittest.cc ./

h264_sps_svc_extension_parser_fuzzer.cc: ../test/h264_sps_svc_extension_parser_unittest.cc
	./converter.py ../test/h264_sps_svc_extension_parser_unittest.cc ./

h264_bitstream_parser_fuzzer.cc: ../test/h264_bitstream_parser_unittest.cc
	./converter.py ../test/h264_bitstream_parser_unittest.cc ./

h264_prefix_nal_unit_parser_fuzzer.cc: ../test/h264_prefix_nal_unit_parser_unittest.cc
	./converter.py ../test/h264_prefix_nal_unit_parser_unittest.cc ./

h264_nal_unit_header_svc_extension_parser_fuzzer.cc: ../test/h264_nal_unit_header_svc_extension_parser_unittest.cc
	./converter.py ../test/h264_nal_unit_header_svc_extension_parser_unittest.cc ./

h264_nal_unit_parser_fuzzer.cc: ../test/h264_nal_unit_parser_unittest.cc
	./converter.py ../test/h264_nal_unit_parser_unittest.cc ./



RUNS ?= 1000000

# Extra libFuzzer flags. Fork mode by default: without it libFuzzer exits
# on the first finding, so one bug ends that target's run. Note that either
# way a crash only ends that one target, never the campaign, because the
# recipe below is prefixed with -.
#
# The cost is that -runs stops being exact, because the parent only checks
# the total at job boundaries; expect it to overshoot by 3-30x. When the run
# count has to be exact, turn it off with FUZZ_FLAGS= on the command line.
#
# Use FUZZ_FLAGS= as well whenever a target is new, or already known to be
# crashing, and the point of the run is to get a reproducer rather than
# coverage:
#
#   * Fork mode buys the rest of the budget on a target that has already
#     failed, so it finds more. Injecting one bug and running every target
#     at RUNS=300, fork mode flagged 4 targets against 3 without it.
#   * It pays for that in artifacts. The same single bug produced 185
#     artifacts under fork mode and 3 without, all of which replayed to one
#     site, so triage means replaying each one and grouping on the failure
#     location.
#   * Worse for a reproducer, a crash in the fork parent, before any child
#     runs, prints its AddressSanitizer report and exits without writing an
#     artifact at all. The input is then gone: the seed libFuzzer prints is
#     the parent's, and replaying it does not reproduce a child's work.
#
# So: fork for soaks, FUZZ_FLAGS= when chasing something.
FUZZ_FLAGS ?= -fork=1 -ignore_crashes=1

# libFuzzer picks a random seed per run and prints it. A finding that does
# not become an artifact is only reproducible if that seed is known, so it
# is worth reading out of the log and replaying with SEED=<n>. Empty by
# default, which leaves libFuzzer's own randomness in place.
SEED ?=
SEED_FLAG = $(if $(SEED),-seed=$(SEED),)

FUZZERS = \
    h264_common_fuzzer \
    h264_sps_parser_fuzzer \
    h264_hrd_parameters_parser_fuzzer \
    h264_vui_parameters_parser_fuzzer \
    h264_pps_parser_fuzzer \
    h264_ref_pic_list_modification_parser_fuzzer \
    h264_pred_weight_table_parser_fuzzer \
    h264_dec_ref_pic_marking_parser_fuzzer \
    h264_rtp_single_parser_fuzzer \
    h264_rtp_parser_fuzzer \
    h264_rtp_stapa_parser_fuzzer \
    h264_rtp_fua_parser_fuzzer \
    h264_slice_header_parser_fuzzer \
    h264_slice_layer_without_partitioning_rbsp_parser_fuzzer \
    h264_sps_extension_parser_fuzzer \
    h264_subset_sps_parser_fuzzer \
    h264_sps_svc_extension_parser_fuzzer \
    h264_bitstream_parser_fuzzer \
    h264_prefix_nal_unit_parser_fuzzer \
    h264_nal_unit_header_svc_extension_parser_fuzzer \
    h264_nal_unit_parser_fuzzer

.PHONY: fuzz $(FUZZERS)

fuzz: $(FUZZERS)

# Failing inputs go to artifacts/, not into corpus/. libFuzzer executes
# every file in the corpus directory at startup, and separately writes new
# coverage-increasing inputs back into it, so pointing -artifact_prefix at
# that same directory means a target that crashes once replays its own
# crash as a seed and never starts again. Keeping them apart leaves corpus/
# as the growing input set and artifacts/ as the list of things to act on.
# Neither mkdir has a - prefix: libFuzzer refuses to start when either the
# artifact directory or the corpus directory is missing, reporting 'ERROR:
# The required directory "..." does not exist', so failing here is better.
# corpus/$@ is created rather than assumed because a target added to
# FUZZERS has no seeds until its first run.
# Everything is teed to logs/$@.log. artifacts/ is not sufficient on its
# own: in fork mode a crash in the parent, before any child runs, prints
# an AddressSanitizer report and exits without writing an artifact, so the
# run looks clean by every other measure. The log is the only place it
# appears.
$(FUZZERS):
	mkdir -p artifacts/$@ corpus/$@ logs
	-../build/fuzz/$@ -artifact_prefix=artifacts/$@/ corpus/$@/ \
	    -runs=$(RUNS) $(SEED_FLAG) $(FUZZ_FLAGS) 2>&1 | tee logs/$@.log

# What did the last campaign find? Reads the logs, not just artifacts/,
# for the reason above.
.PHONY: fuzz-report
fuzz-report:
	@grep -lE '^==[0-9]+==ERROR|runtime error:|ERROR: libFuzzer' logs/*.log 2>/dev/null \
	    || echo 'logs: no findings'
	@ls artifacts/*/* 2>/dev/null || echo 'artifacts: none'

