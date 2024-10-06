# Set the path to the googletest directory
set(GTEST_DIR ${CMAKE_SOURCE_DIR}/../googletest)

# Include Google Test and Google Mock
include_directories(${GTEST_DIR}/googlemock/include)
include_directories(${GTEST_DIR}/googletest/include)

# Include the core directory for header files
include_directories(${CMAKE_SOURCE_DIR}/core)

# Add the googletest subdirectory
add_subdirectory(${GTEST_DIR} ${CMAKE_BINARY_DIR}/googletest)

# Find all test source files
file(GLOB_RECURSE TEST_SOURCES "*.cpp")

# Create the main test executable that links all tests
add_executable(test_exec ${TEST_SOURCES})

target_include_directories(test_exec PUBLIC
                           ${CMAKE_SOURCE_DIR}/include
                           ${CMAKE_SOURCE_DIR}/core) # Include core headers

target_link_libraries(test_exec PRIVATE cmake_template_core gtest gtest_main pthread)

# Add tests
add_test(NAME test_exec COMMAND test_exec)
