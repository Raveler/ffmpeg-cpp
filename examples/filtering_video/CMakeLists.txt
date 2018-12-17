
add_executable(filtering_video
    filtering_video.cpp
)

set_target_properties(filtering_video PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_include_directories(filtering_video PUBLIC
    ${PROJECT_SOURCE_DIR}/include/ffmpegcpp
    ${PROJECT_SOURCE_DIR}/ffmpeg/include
)

target_link_libraries(filtering_video ffmpegcpp)

set_target_properties(filtering_video PROPERTIES FOLDER examples)
