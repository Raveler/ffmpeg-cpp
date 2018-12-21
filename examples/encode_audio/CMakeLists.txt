
add_executable(encode_audio
    encode_audio.cpp
)

set_target_properties(encode_audio PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_include_directories(encode_audio PUBLIC
    ${PROJECT_SOURCE_DIR}/include/ffmpegcpp
    ${PROJECT_SOURCE_DIR}/ffmpeg/include
)

target_link_libraries(encode_audio ffmpegcpp)

set_target_properties(encode_audio PROPERTIES FOLDER examples)
