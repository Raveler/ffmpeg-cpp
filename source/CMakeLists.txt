
set(_include_dir "${PROJECT_SOURCE_DIR}/include/ffmpeg-cpp")
file(GLOB_RECURSE _headers
	"${_include_dir}/*.h"
)

# Get all of the sources from subdirectories
file(GLOB _src_codecs
	"${CMAKE_CURRENT_SOURCE_DIR}/Codecs/*.cpp"
)
file(GLOB _src_demux
	"${CMAKE_CURRENT_SOURCE_DIR}/Demuxing/*.cpp"
)
file(GLOB _src_mux
	"${CMAKE_CURRENT_SOURCE_DIR}/Muxing/*.cpp"
)
file(GLOB _src_framesinks
	"${CMAKE_CURRENT_SOURCE_DIR}/Frame Sinks/*.cpp"
)
file(GLOB _src_source
	"${CMAKE_CURRENT_SOURCE_DIR}/Sources/*.cpp"
)

list(APPEND _sources ${_src_source} ${_src_gramesinks} ${_src_mux} ${_src_demux} ${_src_codecs})

add_library(ffmpeg-cpp STATIC
	${_headers}
	${_sources}
)

target_include_directories(ffmpeg-cpp
	PUBLIC
	<BUILD_INTERFACE:${_include_dir}>
	<INSTALL_INTERFACE:"include/ffmpeg-cpp">
	${FFMPEG_INCLUDE_DIRS}
	PRIVATE ${_include_dir}
)

target_link_libraries(ffmpeg-cpp
	PUBLIC
	${FFMPEG_LIBRARIES}
)