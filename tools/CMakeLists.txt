# add executable
#add_executable(main main.cc)

option(H264NAL_SMALL_FOOTPRINT, "small footprint build")

if(H264NAL_SMALL_FOOTPRINT)
  message(STATUS "src: small footprint selected")
  add_compile_definitions(SMALL_FOOTPRINT)
else()
  message(STATUS "src: normal footprint selected")
  add_compile_definitions(FDUMP_DEFINE)
  add_compile_definitions(RTP_DEFINE)
  add_compile_definitions(FPRINT_ERRORS)
endif()

add_executable(h264nal-bin h264nal.cc)
target_include_directories(h264nal-bin PUBLIC ../src)
target_link_libraries(h264nal-bin PUBLIC h264nal)
# rename executable using target properties
set_target_properties(h264nal-bin PROPERTIES OUTPUT_NAME h264nal)

add_executable(h264nal.nalu h264nal.nalu.cc)
target_include_directories(h264nal.nalu PUBLIC ../src)
target_link_libraries(h264nal.nalu PUBLIC h264nal)

# getopt port needed on windows
if(WIN32)
  target_include_directories(h264nal-bin PRIVATE ../ya_getopt)
  target_sources(h264nal-bin PRIVATE ../ya_getopt/ya_getopt.c)

  target_include_directories(h264nal.nalu PRIVATE ../ya_getopt)
  target_sources(h264nal.nalu PRIVATE ../ya_getopt/ya_getopt.c)
endif()
