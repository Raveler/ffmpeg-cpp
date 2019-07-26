add_executable(difference
    difference.cpp
)

set_target_properties(difference PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${EXAMPLE_BIN_DIR}
)

target_link_libraries(difference ffmpeg-cpp)

set_target_properties(difference PROPERTIES FOLDER examples)