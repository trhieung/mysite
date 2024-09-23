cmake_minimum_required(VERSION 3.10)
project(cmake_template_core)

# Collect all core source files
file(GLOB_RECURSE CORE_SOURCES "*.cpp")

# Create a shared library
add_library(cmake_template_core_shared SHARED ${CORE_SOURCES})

# Create a static library
add_library(cmake_template_core_static STATIC ${CORE_SOURCES})

# Include directories
target_include_directories(cmake_template_core_shared PUBLIC
                           ${CMAKE_SOURCE_DIR}/include)
target_include_directories(cmake_template_core_static PUBLIC
                           ${CMAKE_SOURCE_DIR}/include)

# Set C++ standard
set_property(TARGET cmake_template_core_shared PROPERTY CXX_STANDARD 17)
set_property(TARGET cmake_template_core_static PROPERTY CXX_STANDARD 17)
set_property(TARGET cmake_template_core_shared PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET cmake_template_core_static PROPERTY CXX_STANDARD_REQUIRED ON)

# Manually set output directories for each build type
set_target_properties(cmake_template_core_shared PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/src/lib/$<CONFIG>"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/src/lib/$<CONFIG>"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/src/lib/$<CONFIG>"
)

set_target_properties(cmake_template_core_static PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/src/lib/$<CONFIG>"
)
