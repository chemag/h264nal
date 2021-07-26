



all: \
    h264_bitstream_parser_fuzzer.cc \
    h264_common_fuzzer.cc \
    h264_dec_ref_pic_marking_parser_fuzzer.cc \
    h264_hrd_parameters_parser_fuzzer.cc \
    h264_nal_unit_parser_fuzzer.cc \
    h264_pps_parser_fuzzer.cc \
    h264_pred_weight_table_parser_fuzzer.cc \
    h264_ref_pic_list_reordering_parser_fuzzer.cc \
    h264_slice_header_parser_fuzzer.cc \
    h264_slice_layer_without_partitioning_rbsp_parser_fuzzer.cc \
    h264_sps_parser_fuzzer.cc \
    h264_vui_parameters_parser_fuzzer.cc

h264_bitstream_parser_fuzzer.cc: ../test/h264_bitstream_parser_unittest.cc
	./converter.py ../test/h264_bitstream_parser_unittest.cc ./

h264_common_fuzzer.cc: ../test/h264_common_unittest.cc
	./converter.py ../test/h264_common_unittest.cc ./

h264_dec_ref_pic_marking_parser_fuzzer.cc: ../test/h264_dec_ref_pic_marking_parser_unittest.cc
	./converter.py ../test/h264_dec_ref_pic_marking_parser_unittest.cc ./

h264_hrd_parameters_parser_fuzzer.cc: ../test/h264_hrd_parameters_parser_unittest.cc
	./converter.py ../test/h264_hrd_parameters_parser_unittest.cc ./

h264_nal_unit_parser_fuzzer.cc: ../test/h264_nal_unit_parser_unittest.cc
	./converter.py ../test/h264_nal_unit_parser_unittest.cc ./

h264_pps_parser_fuzzer.cc: ../test/h264_pps_parser_unittest.cc
	./converter.py ../test/h264_pps_parser_unittest.cc ./

h264_pred_weight_table_parser_fuzzer.cc: ../test/h264_pred_weight_table_parser_unittest.cc
	./converter.py ../test/h264_pred_weight_table_parser_unittest.cc ./

h264_ref_pic_list_reordering_parser_fuzzer.cc: ../test/h264_ref_pic_list_reordering_parser_unittest.cc
	./converter.py ../test/h264_ref_pic_list_reordering_parser_unittest.cc ./

h264_slice_header_parser_fuzzer.cc: ../test/h264_slice_header_parser_unittest.cc
	./converter.py ../test/h264_slice_header_parser_unittest.cc ./

h264_slice_layer_without_partitioning_rbsp_parser_fuzzer.cc: ../test/h264_slice_layer_without_partitioning_rbsp_parser_unittest.cc
	./converter.py ../test/h264_slice_layer_without_partitioning_rbsp_parser_unittest.cc ./

h264_sps_parser_fuzzer.cc: ../test/h264_sps_parser_unittest.cc
	./converter.py ../test/h264_sps_parser_unittest.cc ./

h264_vui_parameters_parser_fuzzer.cc: ../test/h264_vui_parameters_parser_unittest.cc
	./converter.py ../test/h264_vui_parameters_parser_unittest.cc ./

RUNS?=1000000

fuzz:
	-../build/fuzz/h264_bitstream_parser_fuzzer -artifact_prefix=corpus/h264_bitstream_parser_fuzzer/ corpus/h264_bitstream_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h264_common_fuzzer -artifact_prefix=corpus/h264_common_fuzzer/ corpus/h264_common_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h264_dec_ref_pic_marking_parser_fuzzer -artifact_prefix=corpus/h264_dec_ref_pic_marking_parser_fuzzer/ corpus/h264_dec_ref_pic_marking_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h264_hrd_parameters_parser_fuzzer -artifact_prefix=corpus/h264_hrd_parameters_parser_fuzzer/ corpus/h264_hrd_parameters_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h264_nal_unit_parser_fuzzer -artifact_prefix=corpus/h264_nal_unit_parser_fuzzer/ corpus/h264_nal_unit_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h264_pps_parser_fuzzer -artifact_prefix=corpus/h264_pps_parser_fuzzer/ corpus/h264_pps_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h264_pred_weight_table_parser_fuzzer -artifact_prefix=corpus/h264_pred_weight_table_parser_fuzzer/ corpus/h264_pred_weight_table_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h264_ref_pic_list_reordering_parser_fuzzer -artifact_prefix=corpus/h264_ref_pic_list_reordering_parser_fuzzer/ corpus/h264_ref_pic_list_reordering_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h264_slice_header_parser_fuzzer -artifact_prefix=corpus/h264_slice_header_parser_fuzzer/ corpus/h264_slice_header_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h264_slice_layer_without_partitioning_rbsp_parser_fuzzer -artifact_prefix=corpus/h264_slice_layer_without_partitioning_rbsp_parser_fuzzer/ corpus/h264_slice_layer_without_partitioning_rbsp_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h264_sps_parser_fuzzer -artifact_prefix=corpus/h264_sps_parser_fuzzer/ corpus/h264_sps_parser_fuzzer/ -runs=$(RUNS)
	-../build/fuzz/h264_vui_parameters_parser_fuzzer -artifact_prefix=corpus/h264_vui_parameters_parser_fuzzer/ corpus/h264_vui_parameters_parser_fuzzer/ -runs=$(RUNS)