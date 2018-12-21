
add_executable(remuxing
    remuxing.cpp
)

set_target_properties(remuxing PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_include_directories(remuxing PUBLIC
    ${PROJECT_SOURCE_DIR}/include/ffmpegcpp
    ${PROJECT_SOURCE_DIR}/ffmpeg/include
)

target_link_libraries(remuxing ffmpegcpp)

set_target_properties(remuxing PROPERTIES FOLDER examples)
