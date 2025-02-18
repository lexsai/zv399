add_subdirectory(SDL)
add_subdirectory(glad)
add_subdirectory(glm)
add_subdirectory(json)

add_library(vendor INTERFACE)
target_link_libraries(vendor INTERFACE SDL3::SDL3 glad glm::glm-header-only nlohmann_json::nlohmann_json)
target_include_directories(vendor INTERFACE .)
