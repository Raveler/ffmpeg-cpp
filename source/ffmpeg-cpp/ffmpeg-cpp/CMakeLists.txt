cmake_minimum_required(VERSION 3.1)

project(ffmpegCppLib)

add_library(${PROJECT_NAME})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_sources(${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/AudioFormatConverter.cpp   
		${CMAKE_CURRENT_SOURCE_DIR}/FFmpegException.cpp  
		${CMAKE_CURRENT_SOURCE_DIR}/OpenCodec.h
		${CMAKE_CURRENT_SOURCE_DIR}/AudioFormatConverter.h     
		${CMAKE_CURRENT_SOURCE_DIR}/FFmpegException.h    
		${CMAKE_CURRENT_SOURCE_DIR}/std.h
		${CMAKE_CURRENT_SOURCE_DIR}/CodecDeducer.cpp           
		${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg.h 
		${CMAKE_CURRENT_SOURCE_DIR}/VideoFormatConverter.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/CodecDeducer.h
		${CMAKE_CURRENT_SOURCE_DIR}/FrameContainer.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/VideoFormatConverter.h
		${CMAKE_CURRENT_SOURCE_DIR}/ConvertedAudioProcessor.h
		${CMAKE_CURRENT_SOURCE_DIR}/FrameContainer.h
		${CMAKE_CURRENT_SOURCE_DIR}/ffmpegcpp.h
		${CMAKE_CURRENT_SOURCE_DIR}/OpenCodec.cpp
		
		${CMAKE_CURRENT_SOURCE_DIR}/Codecs/AudioCodec.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Codecs/Codec.h
		${CMAKE_CURRENT_SOURCE_DIR}/Codecs/PNGCodec.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Codecs/VideoCodec.h
		${CMAKE_CURRENT_SOURCE_DIR}/Codecs/AudioCodec.h
		${CMAKE_CURRENT_SOURCE_DIR}/Codecs/JPGCodec.cpp  
		${CMAKE_CURRENT_SOURCE_DIR}/Codecs/PNGCodec.h 
		${CMAKE_CURRENT_SOURCE_DIR}/Codecs/VP9Codec.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Codecs/Codec.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Codecs/JPGCodec.h
		${CMAKE_CURRENT_SOURCE_DIR}/Codecs/VideoCodec.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Codecs/VP9Codec.h
		
		${CMAKE_CURRENT_SOURCE_DIR}/codecs/H264NVEncCodec.cpp  
		${CMAKE_CURRENT_SOURCE_DIR}/codecs/H264NVEncCodec.h  
		${CMAKE_CURRENT_SOURCE_DIR}/codecs/H265NVEncCodec.cpp  
		${CMAKE_CURRENT_SOURCE_DIR}/codecs/H265NVEncCodec.h
		
		${CMAKE_CURRENT_SOURCE_DIR}/Demuxing/AudioInputStream.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Demuxing/InputStream.cpp  
		${CMAKE_CURRENT_SOURCE_DIR}/Demuxing/StreamData.cpp  
		${CMAKE_CURRENT_SOURCE_DIR}/Demuxing/VideoInputStream.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Demuxing/AudioInputStream.h
		${CMAKE_CURRENT_SOURCE_DIR}/Demuxing/InputStream.h
		${CMAKE_CURRENT_SOURCE_DIR}/Demuxing/StreamData.h
		${CMAKE_CURRENT_SOURCE_DIR}/Demuxing/VideoInputStream.h
		
		${CMAKE_CURRENT_SOURCE_DIR}/Frame\ Sinks/AudioEncoder.cpp  
		${CMAKE_CURRENT_SOURCE_DIR}/Frame\ Sinks/Filter.h
		${CMAKE_CURRENT_SOURCE_DIR}/Frame\ Sinks/OneInputFrameSink.h
		${CMAKE_CURRENT_SOURCE_DIR}/Frame\ Sinks/AudioEncoder.h
		${CMAKE_CURRENT_SOURCE_DIR}/Frame\ Sinks/FrameSink.h
		${CMAKE_CURRENT_SOURCE_DIR}/Frame\ Sinks/VideoEncoder.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Frame\ Sinks/AudioFilter.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Frame\ Sinks/FrameSinkStream.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Frame\ Sinks/VideoEncoder.h
		${CMAKE_CURRENT_SOURCE_DIR}/Frame\ Sinks/AudioFilter.h
		${CMAKE_CURRENT_SOURCE_DIR}/Frame\ Sinks/FrameSinkStream.h
		${CMAKE_CURRENT_SOURCE_DIR}/Frame\ Sinks/VideoFilterInput.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Frame\ Sinks/AudioFrameSink.h
		${CMAKE_CURRENT_SOURCE_DIR}/Frame\ Sinks/FrameWriter.h
		${CMAKE_CURRENT_SOURCE_DIR}/Frame\ Sinks/VideoFilterInput.h
		${CMAKE_CURRENT_SOURCE_DIR}/Frame\ Sinks/Filter.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Frame\ Sinks/OneInputFrameSink.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Frame\ Sinks/VideoFrameSink.h
		
		${CMAKE_CURRENT_SOURCE_DIR}/Info/AudioStreamInfo.h
		${CMAKE_CURRENT_SOURCE_DIR}/Info/ContainerInfo.h
		${CMAKE_CURRENT_SOURCE_DIR}/Info/VideoStreamInfo.h
		
		${CMAKE_CURRENT_SOURCE_DIR}/Muxing/AudioOutputStream.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Muxing/Muxer.cpp 
		${CMAKE_CURRENT_SOURCE_DIR}/Muxing/OutputStream.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Muxing/VideoOutputStream.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Muxing/AudioOutputStream.h
		${CMAKE_CURRENT_SOURCE_DIR}/Muxing/Muxer.h
		${CMAKE_CURRENT_SOURCE_DIR}/Muxing/OutputStream.h
		${CMAKE_CURRENT_SOURCE_DIR}/Muxing/VideoOutputStream.h
		
		${CMAKE_CURRENT_SOURCE_DIR}/Sources/Demuxer.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Sources/RawAudioDataSource.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Sources/RawVideoDataSource.h
		${CMAKE_CURRENT_SOURCE_DIR}/Sources/Demuxer.h
		${CMAKE_CURRENT_SOURCE_DIR}/Sources/RawAudioDataSource.h
		${CMAKE_CURRENT_SOURCE_DIR}/Sources/RawVideoFileSource.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Sources/EncodedFileSource.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Sources/RawAudioFileSource.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Sources/RawVideoFileSource.h
		${CMAKE_CURRENT_SOURCE_DIR}/Sources/EncodedFileSource.h
		${CMAKE_CURRENT_SOURCE_DIR}/Sources/RawAudioFileSource.h
		${CMAKE_CURRENT_SOURCE_DIR}/Sources/InputSource.h
		${CMAKE_CURRENT_SOURCE_DIR}/Sources/RawVideoDataSource.cpp
		
		)
		
target_include_directories(${PROJECT_NAME}
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/Codecs	
)		

target_link_libraries(${PROJECT_NAME} 
	PRIVATE
		 ${AVCODEC_LIBRARY} 
		 ${AVFORMAT_LIBRARY} 
		 ${AVUTIL_LIBRARY} 
		 ${AVDEVICE_LIBRARY}
		 ${RESAMPLE_LIBRARY}
		 ${SWSCALE_LIBRARY}		 
		)