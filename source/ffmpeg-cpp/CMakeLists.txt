cmake_minimum_required(VERSION 3.5)
project(ffmpeg-cpp)

SET(CMAKE_CXX_STANDARD 11)

set (CXX_FLAGS
        -g
        -fPIC
        -Wall
        -Wno-unused-parameter
        -Wno-unused-function
        -Wunused-variable
        -Wunused-value
        -Wshadow
        -Wcast-qual
        -Wcast-align
        -Wwrite-strings
        -Wsign-compare
        -Winvalid-pch
        -fms-extensions
        -Wfloat-equal
        -Wextra
        -std=c++11
        )

SET(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g -ggdb -D_DEBUG")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -ggdb -DNDEBUG")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/3rd/ffmpeg/include)
LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/3rd/ffmpeg/lib)

include_directories(ffmpeg-cpp)

file(GLOB_RECURSE ffmpeg-cpp-srcs ffmpeg-cpp/*.cpp *.h)

add_library(ffmpeg-cpp ${ffmpeg-cpp-srcs})
target_link_libraries(ffmpeg-cpp avcodec avdevice avfilter avformat avutil postproc swresample swscale)