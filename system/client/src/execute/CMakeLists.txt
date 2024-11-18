# src/execute/CMakeLists.txt

# Option to select static or shared library, default to OFF (static)
option(USE_SHARED_LIB "Use shared library for core" OFF)

# Collect all .cpp files in the execute directory
file(GLOB EXECUTE_SOURCES *.cpp)

# Define the output directory for the executables based on the build type
set(EXECUTABLE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE})

# Create the output directory if it doesn't exist
file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_DIRECTORY})

# Create executables for each .cpp file
foreach(SOURCE ${EXECUTE_SOURCES})
    # Get the filename without the extension
    get_filename_component(EXEC_NAME ${SOURCE} NAME_WE)

    # Add an executable for each source file
    add_executable(${EXEC_NAME} ${SOURCE})

    # Link libraries based on the user's choice
    if(USE_SHARED_LIB)
        target_link_libraries(${EXEC_NAME} PRIVATE core_shared CURL::libcurl Threads::Threads OpenSSL::SSL OpenSSL::Crypto)
    else()
        target_link_libraries(${EXEC_NAME} PRIVATE core_static CURL::libcurl Threads::Threads OpenSSL::SSL OpenSSL::Crypto)
    endif()

    # Set the output directory for each executable
    set_target_properties(${EXEC_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_DIRECTORY}
    )

    # Include paths
    target_include_directories(${EXEC_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
endforeach()
