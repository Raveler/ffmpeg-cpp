cmake_minimum_required(VERSION 3.0.0)
  
project(ffmpeg-cpp CXX)
  
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
  
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE OFF)
#set(CMAKE_CXX_FLAGS "-Wall -Wextra")
#set(CMAKE_CXX_FLAGS_DEBUG "-g")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")

file(GLOB_RECURSE INC source/*.h)
file(GLOB_RECURSE SRC source/*.cpp)

add_library(${PROJECT_NAME} ${SRC}) 

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/source/ffmpeg-cpp/)

target_link_libraries(${PROJECT_NAME} ffmpeg)

