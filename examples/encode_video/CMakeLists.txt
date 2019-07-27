add_executable(encode_video
    encode_video.cpp
)

target_link_libraries(encode_video ffmpeg-cpp)

set_target_properties(encode_video
    PROPERTIES
        FOLDER examples
        RUNTIME_OUTPUT_DIRECTORY ${EXAMPLE_BIN_DIR}
)