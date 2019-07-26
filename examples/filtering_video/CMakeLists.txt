add_executable(filtering_video
    filtering_video.cpp
)

set_target_properties(filtering_video PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${EXAMPLE_BIN_DIR}
)

target_link_libraries(filtering_video ffmpeg-cpp)

set_target_properties(filtering_video PROPERTIES FOLDER examples)