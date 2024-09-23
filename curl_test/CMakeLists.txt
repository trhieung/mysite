# Set the minimum version of CMake
cmake_minimum_required(VERSION 3.10)

# Project name
project(MyProject)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories for headers
include_directories(${CMAKE_SOURCE_DIR}/../indicators/single_include/indicators)
include_directories(${CMAKE_SOURCE_DIR}/../curl-8.10.1/include)

# # Path to libcurl (adjust this to the actual location of libcurl.so)
set(CURL_LIBRARY ${CMAKE_SOURCE_DIR}/../curl-8.10.1/lib/.libs/libcurl.so)

# Add the executable
add_executable(curl_demo main.cpp)

# Link the executable with the libcurl library
target_link_libraries(curl_demo ${CURL_LIBRARY})
