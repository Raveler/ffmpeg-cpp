add_executable(encode_audio
    encode_audio.cpp
)

set_target_properties(encode_audio PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${EXAMPLE_BIN_DIR}
)

target_link_libraries(encode_audio ffmpeg-cpp)

set_target_properties(encode_audio PROPERTIES FOLDER examples)