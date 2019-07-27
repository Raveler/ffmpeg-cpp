add_executable(print_info
    print_info.cpp
)

target_link_libraries(print_info ffmpeg-cpp)

set_target_properties(print_info
    PROPERTIES
        FOLDER examples
        RUNTIME_OUTPUT_DIRECTORY ${EXAMPLE_BIN_DIR}
)