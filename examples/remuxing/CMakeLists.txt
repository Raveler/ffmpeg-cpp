add_executable(remuxing
    remuxing.cpp
)

target_link_libraries(remuxing ffmpeg-cpp)

set_target_properties(remuxing
    PROPERTIES
        FOLDER examples
        RUNTIME_OUTPUT_DIRECTORY ${EXAMPLE_BIN_DIR}
)