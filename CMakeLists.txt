cmake_minimum_required(VERSION 3.16)
project(zv399)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
# add_custom_target(force_create_buildsystem ALL
#     COMMAND cmake -S .. -B .
# )

add_subdirectory(vendor)
add_subdirectory(src)

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(${PROJECT_NAME} copy_assets)


option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
