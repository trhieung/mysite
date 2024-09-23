# Set the minimum version of CMake
cmake_minimum_required(VERSION 3.10)

# Project name
project(MyProject)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
include_directories(../indicators/single_include/indicators)

# Add the executable
add_executable(indicator_demo main.cpp)
