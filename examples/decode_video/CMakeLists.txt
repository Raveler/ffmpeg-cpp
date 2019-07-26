add_executable(decode_video
    decode_video.cpp
)

set_target_properties(decode_video PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${EXAMPLE_BIN_DIR}
)

target_link_libraries(decode_video ffmpeg-cpp)

set_target_properties(decode_video PROPERTIES FOLDER examples)