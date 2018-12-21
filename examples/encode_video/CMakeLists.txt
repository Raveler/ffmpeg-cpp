
add_executable(encode_video
    encode_video.cpp
)

set_target_properties(encode_video PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_include_directories(encode_video PUBLIC
    ${PROJECT_SOURCE_DIR}/include/ffmpegcpp
    ${PROJECT_SOURCE_DIR}/ffmpeg/include
)

target_link_libraries(encode_video ffmpegcpp)

set_target_properties(encode_video PROPERTIES FOLDER examples)
