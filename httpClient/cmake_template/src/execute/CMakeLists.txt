# Specify the minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Automatically detect all .cpp files in the current directory (src/execute)
file(GLOB MAIN_SOURCES
    "*.cpp"
)

# Create executables for each .cpp file
foreach(SOURCE ${MAIN_SOURCES})
    # Get the filename without the extension
    get_filename_component(EXEC_NAME ${SOURCE} NAME_WE)
    
    # Add the executable
    add_executable(${EXEC_NAME} ${SOURCE})

    # Set C++ standard for the executable
    set_property(TARGET ${EXEC_NAME} PROPERTY CXX_STANDARD 17)
    set_property(TARGET ${EXEC_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

    # Include the directory for the headers
    target_include_directories(${EXEC_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../include)
    target_include_directories(${EXEC_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/core)

    # Link against the core library (use shared or static as needed)
    # target_link_libraries(${EXEC_NAME} PRIVATE cmake_template_core_shared)  # Use shared library
    target_link_libraries(${EXEC_NAME} PRIVATE cmake_template_core_static -pthread)  # Use static library if needed

    # Set output directory for executables based on build type
    set_target_properties(${EXEC_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/src/execute/$<CONFIG>"
    )
endforeach()
