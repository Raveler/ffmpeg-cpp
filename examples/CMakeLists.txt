# Viariable to set examples output directory
set(EXAMPLE_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}")


add_subdirectory("decode_audio")
add_subdirectory("decode_video")
add_subdirectory("demo")
add_subdirectory("difference")
add_subdirectory("encode_audio")
add_subdirectory("encode_video")
add_subdirectory("filtering_audio")
add_subdirectory("filtering_video")
add_subdirectory("print_info")
add_subdirectory("remuxing")

add_subdirectory("samples")