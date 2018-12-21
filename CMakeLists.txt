cmake_minimum_required(VERSION 3.5)

project(ffmpegcpp VERSION 1.0.0)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

file(GLOB_RECURSE ffmpegcpp_files "include/ffmpegcpp/*.h" "src/*.cpp")
file(GLOB_RECURSE header_files "include/ffmpegcpp/*.h")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${ffmpegcpp_files})

add_library(ffmpegcpp STATIC ${ffmpegcpp_files})

set_target_properties(ffmpegcpp PROPERTIES 
    VERSION ${PROJECT_VERSION}
    CXX_STANDARD 17
)

set_target_properties(ffmpegcpp PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

target_include_directories(ffmpegcpp PRIVATE 
    include/ffmpegcpp
    ffmpeg/include
)

target_link_libraries(ffmpegcpp
    ${PROJECT_SOURCE_DIR}/ffmpeg/lib/avcodec.lib
    ${PROJECT_SOURCE_DIR}/ffmpeg/lib/avdevice.lib
    ${PROJECT_SOURCE_DIR}/ffmpeg/lib/avfilter.lib
    ${PROJECT_SOURCE_DIR}/ffmpeg/lib/avformat.lib
    ${PROJECT_SOURCE_DIR}/ffmpeg/lib/avutil.lib
    ${PROJECT_SOURCE_DIR}/ffmpeg/lib/postproc.lib
    ${PROJECT_SOURCE_DIR}/ffmpeg/lib/swresample.lib
    ${PROJECT_SOURCE_DIR}/ffmpeg/lib/swscale.lib
)

install(TARGETS ffmpegcpp
    ARCHIVE DESTINATION ${CMAKE_BINARY_DIR}/lib
    LIBRARY DESTINATION ${CMAKE_BINARY_DIR}/lib
    PUBLIC_HEADER DESTINATION ${CMAKE_BINARY_DIR}/lib/include
)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/ffmpeg/bin/ DESTINATION ${CMAKE_BINARY_DIR}/bin/Release FILES_MATCHING PATTERN "*.dll")
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/samples/ DESTINATION ${CMAKE_BINARY_DIR}/bin/Release/samples)

add_subdirectory(examples)
