add_executable(filtering_audio
    filtering_audio.cpp
)

set_target_properties(filtering_audio PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${EXAMPLE_BIN_DIR}
)

target_link_libraries(filtering_audio ffmpeg-cpp)

set_target_properties(filtering_audio PROPERTIES FOLDER examples)