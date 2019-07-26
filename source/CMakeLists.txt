

# Get all header files 
set(_include_dir "${PROJECT_SOURCE_DIR}/include/ffmpeg-cpp")
file(GLOB_RECURSE _headers
	"${_include_dir}/*.h"
)

# Get all of the sources from subdirectories
set(_src "")
include("${CMAKE_CURRENT_LIST_DIR}/sources.cmake")

# Group every source file by folder structure
foreach(_source IN ITEMS ${_src})
    get_filename_component(_source_path "${_source}" PATH)
	if(IS_ABSOLUTE ${_source})
		file(RELATIVE_PATH _source_path "${CMAKE_CURRENT_SOURCE_DIR}" "${_source_path}")
    endif()
	string(REPLACE "/" "\\" _group_path "${_source_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()


add_library(ffmpeg-cpp STATIC
	${_headers}
	${_src}
)

target_include_directories(ffmpeg-cpp
	PUBLIC
		$<BUILD_INTERFACE:${_include_dir}>
		$<INSTALL_INTERFACE:"include/ffmpeg-cpp">
		${FFMPEG_INCLUDE_DIRS}
	PRIVATE
		${_include_dir}
)

target_link_libraries(ffmpeg-cpp
	PUBLIC
		${FFMPEG_LIBRARIES}
)