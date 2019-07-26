add_executable(demo
    demo.cpp
	GeneratedAudioSource.cpp
	GeneratedAudioSource.h
	GeneratedVideoSource.cpp
	GeneratedVideoSource.h
)

set_target_properties(demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${EXAMPLE_BIN_DIR}
)

target_link_libraries(demo ffmpeg-cpp)

set_target_properties(demo PROPERTIES FOLDER examples)