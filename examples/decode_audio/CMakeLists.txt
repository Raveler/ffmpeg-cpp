add_executable(decode_audio
    decode_audio.cpp
)

target_link_libraries(decode_audio ffmpeg-cpp)

set_target_properties(decode_audio
    PROPERTIES
        FOLDER examples
        RUNTIME_OUTPUT_DIRECTORY ${EXAMPLE_BIN_DIR}
)