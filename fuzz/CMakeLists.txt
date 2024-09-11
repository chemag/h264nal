
include_directories(PUBLIC ../include)

include_directories(PUBLIC ../src)

# MacOS requirements
include_directories(PUBLIC /usr/local/include)
link_directories(/usr/local/lib)

add_compile_options(-fsanitize=address,fuzzer,undefined)
add_link_options(-fsanitize=address,fuzzer,undefined)

function(add_fuzzer TARGET SOURCE)
  if(NOT WIN32)
    add_executable(${TARGET} ${SOURCE})
    target_link_libraries(${TARGET} PUBLIC h264nal)
  endif()
endfunction()

add_fuzzer(h264_common_fuzzer h264_common_fuzzer.cc)

add_fuzzer(h264_sps_parser_fuzzer h264_sps_parser_fuzzer.cc)

add_fuzzer(h264_hrd_parameters_parser_fuzzer h264_hrd_parameters_parser_fuzzer.cc)

add_fuzzer(h264_vui_parameters_parser_fuzzer h264_vui_parameters_parser_fuzzer.cc)

add_fuzzer(h264_pps_parser_fuzzer h264_pps_parser_fuzzer.cc)

add_fuzzer(h264_ref_pic_list_modification_parser_fuzzer h264_ref_pic_list_modification_parser_fuzzer.cc)

add_fuzzer(h264_pred_weight_table_parser_fuzzer h264_pred_weight_table_parser_fuzzer.cc)

add_fuzzer(h264_dec_ref_pic_marking_parser_fuzzer h264_dec_ref_pic_marking_parser_fuzzer.cc)

if(H264NAL_SMALL_FOOTPRINT)
  message(STATUS "src: small footprint selected")
else()
  message(STATUS "src: normal footprint selected")

  add_fuzzer(h264_rtp_single_parser_fuzzer h264_rtp_single_parser_fuzzer.cc)

  add_fuzzer(h264_rtp_parser_fuzzer h264_rtp_parser_fuzzer.cc)

  add_fuzzer(h264_rtp_stapa_parser_fuzzer h264_rtp_stapa_parser_fuzzer.cc)

  add_fuzzer(h264_rtp_fua_parser_fuzzer h264_rtp_fua_parser_fuzzer.cc)
endif()

add_fuzzer(h264_slice_header_parser_fuzzer h264_slice_header_parser_fuzzer.cc)

add_fuzzer(h264_slice_layer_without_partitioning_rbsp_parser_fuzzer h264_slice_layer_without_partitioning_rbsp_parser_fuzzer.cc)

add_fuzzer(h264_sps_extension_parser_fuzzer h264_sps_extension_parser_fuzzer.cc)

add_fuzzer(h264_subset_sps_parser_fuzzer h264_subset_sps_parser_fuzzer.cc)

add_fuzzer(h264_sps_svc_extension_parser_fuzzer h264_sps_svc_extension_parser_fuzzer.cc)

add_fuzzer(h264_bitstream_parser_fuzzer h264_bitstream_parser_fuzzer.cc)

add_fuzzer(h264_prefix_nal_unit_parser_fuzzer h264_prefix_nal_unit_parser_fuzzer.cc)

add_fuzzer(h264_nal_unit_header_svc_extension_parser_fuzzer h264_nal_unit_header_svc_extension_parser_fuzzer.cc)

add_fuzzer(h264_nal_unit_parser_fuzzer h264_nal_unit_parser_fuzzer.cc)

if(H264NAL_SMALL_FOOTPRINT)
  message(STATUS "fuzz: small footprint selected")

else()
  message(STATUS "fuzz: normal footprint selected")
  add_compile_definitions(RTP_DEFINE)
endif()
