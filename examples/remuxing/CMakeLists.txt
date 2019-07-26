add_executable(remuxing
    remuxing.cpp
)

set_target_properties(remuxing PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${EXAMPLE_BIN_DIR}
)

target_link_libraries(remuxing ffmpeg-cpp)

set_target_properties(remuxing PROPERTIES FOLDER examples)