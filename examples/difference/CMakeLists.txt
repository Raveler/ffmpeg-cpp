add_executable(difference
    difference.cpp
)

target_link_libraries(difference ffmpeg-cpp)

set_target_properties(difference
    PROPERTIES
        FOLDER examples
        RUNTIME_OUTPUT_DIRECTORY ${EXAMPLE_BIN_DIR}
)