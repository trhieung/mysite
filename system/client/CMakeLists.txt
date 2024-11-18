cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Output directories for different build types
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/core/libs)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/core/libs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/execute)

# Build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Find and link Curl and Pthread
find_package(CURL REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

# Add subdirectories
add_subdirectory(src/core)
add_subdirectory(src/execute)

# Option to add test subdirectory
option(ENABLE_TEST "Enable building the test subdirectory" OFF)

if(ENABLE_TEST)
  add_subdirectory(src/test)
endif()

# Custom clean target
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/src
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/*.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/*.txt
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/Makefile
    COMMENT "Cleaning up build directory while keeping the bin folder"
)

# Custom release target
add_custom_target(release
    COMMAND ${CMAKE_COMMAND} --build . --target clean-all
    COMMAND ${CMAKE_COMMAND} ..
    COMMAND ${CMAKE_COMMAND} --build .
    COMMENT "Cleaning and building the project with release"
)

# Custom debug target
add_custom_target(debug
    COMMAND ${CMAKE_COMMAND} --build . --target clean-all
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ..
    COMMAND ${CMAKE_COMMAND} --build .
    COMMENT "Cleaning and building the project with debug"
)

# Custom build target
add_custom_target(build
    COMMAND ${CMAKE_COMMAND} --build .
    COMMENT "Cleaning and building the project"
)

# Custom run-test target
add_custom_target(run-test
    COMMAND ${CMAKE_COMMAND} --build . --target clean-all
    COMMAND ${CMAKE_COMMAND} -DENABLE_TEST=ON ..
    COMMAND ${CMAKE_COMMAND} --build .
    COMMENT "Cleaning and building the project"
)