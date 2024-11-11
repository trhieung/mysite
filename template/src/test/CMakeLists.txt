# Set the target directory for third-party includes
set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/include/3-parties")
set(GTEST_DIR "${THIRD_PARTY_DIR}/googletest")
set(GTEST_BUILD_DIR "${GTEST_DIR}/build")

# Ensure the third-party directory exists
file(MAKE_DIRECTORY ${THIRD_PARTY_DIR})

# Clone GoogleTest if not already cloned
if(NOT EXISTS ${GTEST_DIR})
    message(STATUS "Cloning GoogleTest repository...")
    execute_process(
        COMMAND git clone https://github.com/google/googletest.git -b v1.15.2
        WORKING_DIRECTORY ${THIRD_PARTY_DIR}
    )
endif()

# Only build GoogleTest if it hasn't been built already
if(NOT EXISTS ${GTEST_BUILD_DIR})
    message(STATUS "Building GoogleTest...")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -S ${GTEST_DIR} -B ${GTEST_BUILD_DIR}
        COMMAND ${CMAKE_COMMAND} --build ${GTEST_BUILD_DIR}
        WORKING_DIRECTORY ${GTEST_BUILD_DIR}
    )
else()
    message(STATUS "GoogleTest already built. Skipping build.")
endif()

# Add GoogleTest to the project if it was successfully cloned
if(EXISTS ${GTEST_DIR}/CMakeLists.txt)
    add_subdirectory(${GTEST_DIR} ${GTEST_BUILD_DIR})
    include_directories(${GTEST_DIR}/googletest/include)
else()
    message(FATAL_ERROR "Failed to locate GoogleTest after cloning.")
endif()

# Find all test source files
file(GLOB_RECURSE TEST_SOURCES "*.cpp")

# Create the main test executable that links all tests
add_executable(test_exec ${TEST_SOURCES})

target_include_directories(test_exec PUBLIC
                           ${CMAKE_SOURCE_DIR}/include)

# target_link_libraries(test_exec PRIVATE core_shared gtest gtest_main)
target_link_libraries(test_exec PRIVATE core_static gtest gtest_main CURL::libcurl Threads::Threads)

# Add tests
add_test(NAME test_exec COMMAND test_exec)




# # # Set the path to the googletest directory
# # set(GTEST_DIR ${CMAKE_SOURCE_DIR}/../googletest)

# # # Include Google Test and Google Mock
# # include_directories(${GTEST_DIR}/googlemock/include)
# # include_directories(${GTEST_DIR}/googletest/include)

# # # Include the core directory for header files
# # include_directories(${CMAKE_SOURCE_DIR}/core)

# # # Add the googletest subdirectory
# # add_subdirectory(${GTEST_DIR} ${CMAKE_BINARY_DIR}/googletest)

# # Find all test source files
# file(GLOB_RECURSE TEST_SOURCES "*.cpp")

# # Create the main test executable that links all tests
# add_executable(test_exec ${TEST_SOURCES})

# target_include_directories(test_exec PUBLIC
#                            ${CMAKE_SOURCE_DIR}/include)
#                         #    ${CMAKE_SOURCE_DIR}/core) # Include core headers

# target_link_libraries(test_exec PRIVATE cmake_template_core gtest gtest_main pthread)

# # Add tests
# add_test(NAME test_exec COMMAND test_exec)
