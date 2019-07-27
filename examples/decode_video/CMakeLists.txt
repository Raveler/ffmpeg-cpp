add_executable(decode_video
    decode_video.cpp
)

target_link_libraries(decode_video ffmpeg-cpp)

set_target_properties(decode_video
    PROPERTIES
        FOLDER examples
        RUNTIME_OUTPUT_DIRECTORY ${EXAMPLE_BIN_DIR}
)