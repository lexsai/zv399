cmake_minimum_required(VERSION 3.16)
project(zv399)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# add_custom_target(force_create_buildsystem ALL
#     COMMAND cmake -S .. -B .
# )

add_subdirectory(vendor)
add_subdirectory(src)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)