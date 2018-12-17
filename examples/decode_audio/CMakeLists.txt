
add_executable(decode_audio
    decode_audio.cpp
)

set_target_properties(decode_audio PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_include_directories(decode_audio PUBLIC
    ${PROJECT_SOURCE_DIR}/include/ffmpegcpp
    ${PROJECT_SOURCE_DIR}/ffmpeg/include
)

target_link_libraries(decode_audio ffmpegcpp)

set_target_properties(decode_audio PROPERTIES FOLDER examples)
