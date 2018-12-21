
add_executable(decode_video
    decode_video.cpp
)

set_target_properties(decode_video PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_include_directories(decode_video PUBLIC
    ${PROJECT_SOURCE_DIR}/include/ffmpegcpp
    ${PROJECT_SOURCE_DIR}/ffmpeg/include
)

target_link_libraries(decode_video ffmpegcpp)

set_target_properties(decode_video PROPERTIES FOLDER examples)
