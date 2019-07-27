add_executable(demo
    demo.cpp
	GeneratedAudioSource.cpp
	GeneratedAudioSource.h
	GeneratedVideoSource.cpp
	GeneratedVideoSource.h
)

target_link_libraries(demo ffmpeg-cpp)

set_target_properties(demo
    PROPERTIES
        FOLDER examples
        RUNTIME_OUTPUT_DIRECTORY ${EXAMPLE_BIN_DIR}
)