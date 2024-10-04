# add executable
#add_executable(main main.cc)

option(H264NAL_SMALL_FOOTPRINT, "xmall footprint build")

if(H264NAL_SMALL_FOOTPRINT)
  message(STATUS "src: small footprint selected")
  add_compile_definitions(SMALL_FOOTPRINT)
else()
  message(STATUS "src: normal footprint selected")
  add_compile_definitions(FDUMP_DEFINE)
  add_compile_definitions(RTP_DEFINE)
  add_compile_definitions(FPRINT_ERRORS)
endif()

if(H264NAL_SMALL_FOOTPRINT)
  add_library(h264nal
      rtc_common.cc
      h264_common.cc
      h264_utils.cc
      h264_sps_parser.cc
      h264_hrd_parameters_parser.cc
      h264_vui_parameters_parser.cc
      h264_pps_parser.cc
      h264_ref_pic_list_modification_parser.cc
      h264_pred_weight_table_parser.cc
      h264_dec_ref_pic_marking_parser.cc
      h264_slice_header_parser.cc
      h264_slice_header_in_scalable_extension_parser.cc
      h264_slice_layer_without_partitioning_rbsp_parser.cc
      h264_slice_layer_extension_rbsp_parser.cc
      h264_sps_extension_parser.cc
      h264_subset_sps_parser.cc
      h264_sps_svc_extension_parser.cc
      h264_bitstream_parser_state.cc
      h264_bitstream_parser.cc
      h264_prefix_nal_unit_parser.cc
      h264_nal_unit_header_svc_extension_parser.cc
      h264_nal_unit_header_parser.cc
      h264_nal_unit_payload_parser.cc
      h264_nal_unit_parser.cc
)
else()
  add_library(h264nal
      rtc_common.cc
      h264_common.cc
      h264_utils.cc
      h264_sps_parser.cc
      h264_hrd_parameters_parser.cc
      h264_vui_parameters_parser.cc
      h264_pps_parser.cc
      h264_ref_pic_list_modification_parser.cc
      h264_pred_weight_table_parser.cc
      h264_dec_ref_pic_marking_parser.cc
      h264_rtp_single_parser.cc
      h264_rtp_parser.cc
      h264_rtp_stapa_parser.cc
      h264_rtp_fua_parser.cc
      h264_slice_header_parser.cc
      h264_slice_header_in_scalable_extension_parser.cc
      h264_slice_layer_without_partitioning_rbsp_parser.cc
      h264_slice_layer_extension_rbsp_parser.cc
      h264_sps_extension_parser.cc
      h264_subset_sps_parser.cc
      h264_sps_svc_extension_parser.cc
      h264_bitstream_parser_state.cc
      h264_bitstream_parser.cc
      h264_prefix_nal_unit_parser.cc
      h264_nal_unit_header_svc_extension_parser.cc
      h264_nal_unit_header_parser.cc
      h264_nal_unit_payload_parser.cc
      h264_nal_unit_parser.cc
)
endif()

target_include_directories(h264nal PUBLIC ../include/)
if (WIN32)
  target_link_libraries(h264nal PUBLIC wsock32 ws2_32)
endif()

# https://cmake.org/cmake/help/latest/guide/tutorial/index.html#adding-a-version-number-and-configured-header-file
configure_file(config.h.in config.h)

# this allows the 'config.h' file to be used throughout the program
target_include_directories(h264nal PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
