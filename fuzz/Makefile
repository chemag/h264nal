



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

# Extra libFuzzer flags. Empty by default so that -runs is honored exactly:
# in fork mode it degrades to a floor checked at job boundaries, and
# overshoots by 3-30x. For a long soak, where a finding should not end the
# run, pass FUZZ_FLAGS='-fork=1 -ignore_crashes=1' and read RUNS as a
# minimum rather than a count.
FUZZ_FLAGS ?=

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
# The mkdir deliberately has no - prefix: libFuzzer refuses to start when
# the artifact directory is missing, so failing here is better.
$(FUZZERS):
	mkdir -p artifacts/$@
	-../build/fuzz/$@ -artifact_prefix=artifacts/$@/ corpus/$@/ -runs=$(RUNS) $(FUZZ_FLAGS)
