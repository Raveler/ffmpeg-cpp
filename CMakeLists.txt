cmake_minimum_required(VERSION 3.8)

# Don't configure in project root
if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
	message(FATAL_ERROR "Please don't make configuration in-source.")
endif()

project(ffmpeg-cpp VERSION 1.0.0.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

# If FFMPEG_DIR is not specified assume it's in /vendor/ffmpeg
if("${FFMPEG_DIR}" STREQUAL "")
	set(FFMPEG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/ffmpeg")
endif()

find_package(FFMPEG REQUIRED)

# Orginize targets by folder structure
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set C++ standard
set (CMAKE_CXX_STANDARD 11)

add_subdirectory("source")
add_subdirectory("examples")