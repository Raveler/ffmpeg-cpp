
add_executable(demo
    demo.cpp
    GeneratedAudioSource.cpp
    GeneratedVideoSource.cpp
)

set_target_properties(demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_include_directories(demo PUBLIC
    ${PROJECT_SOURCE_DIR}/include/ffmpegcpp
    ${PROJECT_SOURCE_DIR}/ffmpeg/include
)

target_link_libraries(demo ffmpegcpp)

set_target_properties(demo PROPERTIES FOLDER examples)
