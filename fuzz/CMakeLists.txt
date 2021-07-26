# link executable to webrtc library
# Since the webrtc library has public include directories
# we will use those link directories when building `main`
#target_link_libraries(main LINK_PUBLIC webrtc)

include_directories(PUBLIC ../src)
include_directories(PUBLIC ../webrtc)

# MacOS requirements
include_directories(PUBLIC /usr/local/include)
link_directories(/usr/local/lib)

add_compile_options(-fsanitize=address,fuzzer,undefined)
add_link_options(-fsanitize=address,fuzzer,undefined)

#target_include_directories(h264_bitstream_parser_fuzzer PUBLIC /usr/local/include ${CMAKE_CURRENT_SOURCE_DIR} webrtc)

add_executable(h264_bitstream_parser_fuzzer h264_bitstream_parser_fuzzer.cc)
target_link_libraries(h264_bitstream_parser_fuzzer PUBLIC h264nal)

add_executable(h264_common_fuzzer h264_common_fuzzer.cc)
target_link_libraries(h264_common_fuzzer PUBLIC h264nal)

add_executable(h264_dec_ref_pic_marking_parser_fuzzer h264_dec_ref_pic_marking_parser_fuzzer.cc)
target_link_libraries(h264_dec_ref_pic_marking_parser_fuzzer PUBLIC h264nal)

add_executable(h264_hrd_parameters_parser_fuzzer h264_hrd_parameters_parser_fuzzer.cc)
target_link_libraries(h264_hrd_parameters_parser_fuzzer PUBLIC h264nal)

add_executable(h264_nal_unit_parser_fuzzer h264_nal_unit_parser_fuzzer.cc)
target_link_libraries(h264_nal_unit_parser_fuzzer PUBLIC h264nal)

add_executable(h264_pps_parser_fuzzer h264_pps_parser_fuzzer.cc)
target_link_libraries(h264_pps_parser_fuzzer PUBLIC h264nal)

add_executable(h264_pred_weight_table_parser_fuzzer h264_pred_weight_table_parser_fuzzer.cc)
target_link_libraries(h264_pred_weight_table_parser_fuzzer PUBLIC h264nal)

add_executable(h264_ref_pic_list_reordering_parser_fuzzer h264_ref_pic_list_reordering_parser_fuzzer.cc)
target_link_libraries(h264_ref_pic_list_reordering_parser_fuzzer PUBLIC h264nal)

add_executable(h264_slice_header_parser_fuzzer h264_slice_header_parser_fuzzer.cc)
target_link_libraries(h264_slice_header_parser_fuzzer PUBLIC h264nal)

add_executable(h264_slice_layer_without_partitioning_rbsp_parser_fuzzer h264_slice_layer_without_partitioning_rbsp_parser_fuzzer.cc)
target_link_libraries(h264_slice_layer_without_partitioning_rbsp_parser_fuzzer PUBLIC h264nal)

add_executable(h264_sps_parser_fuzzer h264_sps_parser_fuzzer.cc)
target_link_libraries(h264_sps_parser_fuzzer PUBLIC h264nal)

add_executable(h264_vui_parameters_parser_fuzzer h264_vui_parameters_parser_fuzzer.cc)
target_link_libraries(h264_vui_parameters_parser_fuzzer PUBLIC h264nal)

if(H264NAL_SMALL_FOOTPRINT)
  message(STATUS "fuzz: small footprint selected")

else()
  message(STATUS "fuzz: normal footprint selected")
  add_compile_definitions(RTP_DEFINE)
endif()