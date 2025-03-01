add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE main.cpp renderer.cpp)
target_link_libraries(${PROJECT_NAME} vendor)

add_library(game SHARED game.cpp map.cpp)
target_link_libraries(game vendor)
string(RANDOM pdbname)
set_target_properties(game PROPERTIES
  PDB_NAME ${pdbname}
)
