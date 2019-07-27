# FindFFMPEG.cmake
#
#

macro(_ffmpeg_find varname component header)
	find_path(${varname}_INCLUDE_DIR
		NAMES
			"${component}/${header}"
			"lib${component}/${header}"
		HINTS
			"${FFMPEG_DIR}/include"
			"$ENV{FFMPEG_DIR}/include"
			"$ENV{ProgramFiles}/ffmpeg/include"
			"$ENV{ProgramFiles}/FFmpeg/include"
	)
	
	if("${${varname}_INCLUDE_DIR}" STREQUAL "${varname}_INCLUDE_DIR-NOTFOUND")
		message(WARNING "${component} include directories not found")
		set(${varname}_INCLUDE_DIR "")
	endif()
	
	find_library(${varname}_LIBRARY
		NAMES
			"${component}"
			"lib${component}"
		HINTS
			"${FFMPEG_DIR}/lib"
			"$ENV{FFMPEG_DIR}/lib"
			"$ENV{ProgramFiles}/ffmpeg/lib"
			"$ENV{ProgramFiles}/FFmpeg/lib"
	)
	set(${varname}_FOUND ON)
	
	if("${${varname}_LIBRARY}" STREQUAL "${varname}_LIBRARY-NOTFOUND")
		message(WARNING "${component} not found!")
		set(${varname}_FOUND OFF)
		set(${varname}_LIBRARY "")
	else()
		message(STATUS "Found ${component} - ${${varname}_LIBRARY}")
	endif()
	
	
endmacro(_ffmpeg_find)


_ffmpeg_find(FFMPEG_AVFORMAT avformat avformat.h)
_ffmpeg_find(FFMPEG_AVDEVICE avdevice avdevice.h)
_ffmpeg_find(FFMPEG_AVCODEC  avcodec avcodec.h)
_ffmpeg_find(FFMPEG_AVUTIL   avutil  avutil.h)
_ffmpeg_find(FFMPEG_AVFILTER  avfilter avfilter.h)
_ffmpeg_find(FFMPEG_SWRESAMPLE  swresample swresample.h)
_ffmpeg_find(FFMPEG_SWSCALE  swscale swscale.h)
_ffmpeg_find(FFMPEG_POSTPROC  postproc postprocess.h)

list(APPEND FFMPEG_LIBRARIES
	${FFMPEG_AVFORMAT_LIBRARY}
	${FFMPEG_AVDEVICE_LIBRARY}
	${FFMPEG_AVCODEC_LIBRARY}
	${FFMPEG_AVUTIL_LIBRARY}
	${FFMPEG_AVFILTER_LIBRARY}
	${FFMPEG_SWRESAMPLE_LIBRARY}
	${FFMPEG_SWSCALE_LIBRARY}
	${FFMPEG_POSTPROC_LIBRARY}
)

list(APPEND FFMPEG_INCLUDE_DIRS
	${FFMPEG_AVFORMAT_INCLUDE_DIR}
	${FFMPEG_AVDEVICE_INCLUDE_DIR}
	${FFMPEG_AVCODEC_INCLUDE_DIR}
	${FFMPEG_AVUTIL_INCLUDE_DIR}
	${FFMPEG_AVFILTER_INCLUDE_DIR}
	${FFMPEG_SWRESAMPLE_INCLUDE_DIR}
	${FFMPEG_SWSCALE_INCLUDE_DIR}
	${FFMPEG_POSTPROC_INCLUDE_DIR}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FFMPEG
	REQUIRED_VARS FFMPEG_LIBRARIES FFMPEG_INCLUDE_DIRS
)