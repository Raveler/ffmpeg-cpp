cmake_minimum_required(VERSION 3.1)
#
#build using cmake -DCMAKE_BUILD_TYPE=Release .
#
project(ffmpeg-cpp- CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-D__STDC_CONSTANT_MACROS")
set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".so;.a")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)

#if you use ffmepg built from source (configure/make), put here the ffmpeg path  
set (FFMPEG_PATH "../../ffmpeg")

# if you have the package installed on a debian based distro
# should be something like /usr/include/x86_64-linux-gnu/libavcodec/avcodec.h
#
# therefore you should have
#
# find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h  /usr/include/x86_64-linux-gnu )
# find_library(AVCODEC_LIBRARY avcodec /usr/lib/x86_64-linux-gnu )

find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h  $FFMPEG_PATH )
find_library(AVCODEC_LIBRARY avcodec $FFMPEG_PATH/libavcodec )

message("AVCODEC_INCLUDE_DIR: ${AVCODEC_INCLUDE_DIR}")
message("AVCODEC_LIBRARY: ${AVCODEC_LIBRARY}")

find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h $FFMPEG_PATH )
find_library(AVFORMAT_LIBRARY avformat $FFMPEG_PATH/libavformat )

find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h $FFMPEG_PATH )
find_library(AVUTIL_LIBRARY avutil $FFMPEG_PATH/libavutil )

find_path(AVDEVICE_INCLUDE_DIR libavdevice/avdevice.h $FFMPEG_PATH )
find_library(AVDEVICE_LIBRARY avdevice $FFMPEG_PATH/libavdevice )

find_path(SWSCALE_INCLUDE_DIR libswscale/swscale.h $FFMPEG_PATH )
find_library(SWSCALE_LIBRARY swscale $FFMPEG_PATH/libswscale )

find_path(RESAMPLE_INCLUDE_DIR libavresample/avresample.h $FFMPEG_PATH )
find_library(RESAMPLE_LIBRARY swresample $FFMPEG_PATH/libswresample )

find_path(AVFILTER_INCLUDE_DIR libavfilter/avfilter.h  $FFMPEG_PATH )
find_library(AVFILTER_LIBRARY avfilter $FFMPEG_PATH/libavfilter )
message("AVFILTER_INCLUDE_DIR: ${AVFILTER_INCLUDE_DIR}")
message("AVFILTER_LIBRARY: ${AVFILTER_LIBRARY}")

add_subdirectory(ffmpeg-cpp)

add_executable(${PROJECT_NAME}decode_audio)
target_include_directories(${PROJECT_NAME}decode_audio
		PRIVATE
			${AVCODEC_INCLUDE_DIR} 
)

target_link_libraries(${PROJECT_NAME}decode_audio 
		PRIVATE
		# warning the order of libs matters !
		 Threads::Threads
		 ffmpegCppLib
		 ${AVFORMAT_LIBRARY} 
		 ${AVCODEC_LIBRARY} 
		 ${AVUTIL_LIBRARY} 
		 ${AVDEVICE_LIBRARY}
	 	 ZLIB::ZLIB	
	)

target_sources(${PROJECT_NAME}decode_audio
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}/decode_audio/decode_audio.cpp
	)

######################################################################################"	
add_executable(${PROJECT_NAME}decode_video)
target_include_directories(${PROJECT_NAME}decode_video
		PRIVATE
			${AVCODEC_INCLUDE_DIR} 
	)

target_link_libraries(${PROJECT_NAME}decode_video 
		PRIVATE
		# warning the order of libs matters !
		 Threads::Threads
		 ffmpegCppLib
		 ${AVFORMAT_LIBRARY} 
		 ${AVCODEC_LIBRARY} 
		 ${AVUTIL_LIBRARY} 
		 ${AVDEVICE_LIBRARY}
	 	 ZLIB::ZLIB	
	)

target_sources(${PROJECT_NAME}decode_video
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}/decode_video/decode_video.cpp
	)

######################################################################################"	
add_executable(${PROJECT_NAME}demo)
target_include_directories(${PROJECT_NAME}demo
	PRIVATE
		${AVCODEC_INCLUDE_DIR} 
)

target_link_libraries(${PROJECT_NAME}demo 
		PRIVATE
		# warning the order of libs matters !
		 Threads::Threads
		 ffmpegCppLib
		 ${AVFORMAT_LIBRARY} 
		 ${AVCODEC_LIBRARY} 
		 ${AVUTIL_LIBRARY} 
		 ${AVDEVICE_LIBRARY}
		 ${AVFILTER_LIBRARY}		 
	 	 ZLIB::ZLIB	
	)

target_sources(${PROJECT_NAME}demo
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}/demo/demo.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/demo/GeneratedAudioSource.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/demo/GeneratedAudioSource.h
			${CMAKE_CURRENT_SOURCE_DIR}/demo/GeneratedVideoSource.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/demo/GeneratedVideoSource.h
	)

######################################################################################"	
add_executable(${PROJECT_NAME}difference)
target_include_directories(${PROJECT_NAME}difference
	PRIVATE
		${AVCODEC_INCLUDE_DIR} 
)
target_link_libraries(${PROJECT_NAME}difference 
	PRIVATE
	# warning the order of libs matters !
	 Threads::Threads
	 ffmpegCppLib
	 ${AVFORMAT_LIBRARY} 
	 ${AVCODEC_LIBRARY} 
	 ${AVUTIL_LIBRARY} 
	 ${AVDEVICE_LIBRARY}
	 ${AVFILTER_LIBRARY}		 
 	 ZLIB::ZLIB	
	)
	
target_sources(${PROJECT_NAME}difference
	PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}/difference/difference.cpp
	)

######################################################################################"	
add_executable(${PROJECT_NAME}encode_audio)

target_include_directories(${PROJECT_NAME}encode_audio
		PRIVATE
			${AVCODEC_INCLUDE_DIR} 
	)

target_link_libraries(${PROJECT_NAME}encode_audio 
		PRIVATE
		ffmpegCppLib
		# warning the order of libs matters !
		 Threads::Threads
		 ffmpegCppLib
		 ${AVFORMAT_LIBRARY} 
		 ${AVCODEC_LIBRARY} 
		 ${AVUTIL_LIBRARY} 
		 ${AVDEVICE_LIBRARY}
	 	 ZLIB::ZLIB	
	)

target_sources(${PROJECT_NAME}encode_audio
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}/encode_audio/encode_audio.cpp
	)

######################################################################################"	
add_executable(${PROJECT_NAME}encode_video)
target_include_directories(${PROJECT_NAME}encode_video
	PRIVATE
		${AVCODEC_INCLUDE_DIR} 
	)

target_link_libraries(${PROJECT_NAME}encode_video 
		PRIVATE
		# warning the order of libs matters !
		 Threads::Threads
		 ffmpegCppLib
		 ${AVFORMAT_LIBRARY} 
		 ${AVCODEC_LIBRARY} 
		 ${AVUTIL_LIBRARY} 
		 ${AVDEVICE_LIBRARY}
	 	 ZLIB::ZLIB		 
	)

target_sources(${PROJECT_NAME}encode_video
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}/encode_video/encode_video.cpp
	)

######################################################################################"	
add_executable(${PROJECT_NAME}filtering_audio)
target_include_directories(${PROJECT_NAME}filtering_audio
	PRIVATE
			${AVCODEC_INCLUDE_DIR} 
	)

target_link_libraries(${PROJECT_NAME}filtering_audio 
		PRIVATE
		# warning the order of libs matters !
		 Threads::Threads
		 ffmpegCppLib	
		 ${AVFORMAT_LIBRARY} 
		 ${AVCODEC_LIBRARY} 
		 ${AVUTIL_LIBRARY} 
		 ${AVDEVICE_LIBRARY}
		 ${AVFILTER_LIBRARY}
	 	 ZLIB::ZLIB	
	)

target_sources(${PROJECT_NAME}filtering_audio
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}/filtering_audio/filtering_audio.cpp
	)

######################################################################################"	
add_executable(${PROJECT_NAME}filtering_video)
target_include_directories(${PROJECT_NAME}filtering_video
		PRIVATE
			${AVCODEC_INCLUDE_DIR} 
	)

target_link_libraries(${PROJECT_NAME}filtering_video 
		PRIVATE
		# warning the order of libs matters !
		 Threads::Threads
		 ffmpegCppLib
		 ${AVFORMAT_LIBRARY} 
		 ${AVCODEC_LIBRARY} 
		 ${AVUTIL_LIBRARY} 
		 ${AVDEVICE_LIBRARY}
		 ${AVFILTER_LIBRARY}		 
	 	 ZLIB::ZLIB	
	)

target_sources(${PROJECT_NAME}filtering_video
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}/filtering_video/filtering_video.cpp
	)

###########################################################################################
add_executable(${PROJECT_NAME}print_info)
target_include_directories(${PROJECT_NAME}print_info
		PRIVATE
			${AVCODEC_INCLUDE_DIR} 
	)

target_link_libraries(${PROJECT_NAME}print_info 
		PRIVATE
		# warning the order of libs matters !
		 Threads::Threads
		 ffmpegCppLib
		 ${AVFORMAT_LIBRARY} 
		 ${AVCODEC_LIBRARY} 
		 ${AVUTIL_LIBRARY} 
		 ${AVDEVICE_LIBRARY}
	 	 ZLIB::ZLIB	
	)
	
target_sources(${PROJECT_NAME}print_info
	PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}/print_info/print_info.cpp
	)

######################################################################################"	
add_executable(${PROJECT_NAME}remuxing)
target_include_directories(${PROJECT_NAME}remuxing
		PRIVATE
			${AVCODEC_INCLUDE_DIR} 
)

target_link_libraries(${PROJECT_NAME}remuxing 
		PRIVATE
		# warning the order of libs matters !
		 Threads::Threads
		 ffmpegCppLib
		 ${AVFORMAT_LIBRARY} 
		 ${AVCODEC_LIBRARY} 
		 ${AVUTIL_LIBRARY} 
		 ${AVDEVICE_LIBRARY}
	 	 ZLIB::ZLIB	
	)

target_sources(${PROJECT_NAME}remuxing
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}/remuxing/remuxing.cpp
	)	
	