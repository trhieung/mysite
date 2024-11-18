# Collect all .cpp files in src/core directory
file(GLOB CORE_SOURCES *.cpp)

# Define library target for core as both static and shared
add_library(core_static STATIC ${CORE_SOURCES})
add_library(core_shared SHARED ${CORE_SOURCES})

# Specify output directories based on build type
set_target_properties(core_static PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/core/libs/${CMAKE_BUILD_TYPE}
)

set_target_properties(core_shared PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/core/libs/${CMAKE_BUILD_TYPE}
)

# Include paths
target_include_directories(core_static PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(core_shared PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link with libcurl and pthread
target_link_libraries(core_static PRIVATE CURL::libcurl Threads::Threads OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(core_shared PRIVATE CURL::libcurl Threads::Threads OpenSSL::SSL OpenSSL::Crypto)

# Optionally set properties for both libraries
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(core_static PROPERTIES LINKER_LANGUAGE CXX)
    set_target_properties(core_shared PROPERTIES LINKER_LANGUAGE CXX)
endif()
