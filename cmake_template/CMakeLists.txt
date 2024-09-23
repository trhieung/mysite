# ./
cmake_minimum_required(VERSION 3.20)
project(cmake_template LANGUAGES CXX)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
# Add subdirectories
add_subdirectory(src/core)
add_subdirectory(src/execute)

# # Google Test
# enable_testing()
# add_subdirectory(src/test)
