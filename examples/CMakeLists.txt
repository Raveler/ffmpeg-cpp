# Variable to set examples output directory
set(EXAMPLE_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}")

set(examples
remuxing print_info filtering_video filtering_audio encode_video
		encode_audio difference demo decode_video decode_audio
)

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

add_custom_target(samples ALL)
macro(copy_samples dir)
	add_custom_command(
		TARGET samples POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/samples" "${dir}/samples"
	)
endmacro(copy_samples)

copy_samples("${EXAMPLE_BIN_DIR}")

if(MSVC)
	set_target_properties(
		${examples}
		PROPERTIES
		VS_DEBUGGER_WORKING_DIRECTORY "${EXAMPLE_BIN_DIR}"
	)
endif()