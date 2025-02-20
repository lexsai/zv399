#include "game.h"

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>
#include <typeinfo>

static renderer_interface renderer; 

struct map_layer {
  std::vector<int> tiles;
  int height;
  int width;
};
static map_layer floor_layer;

void loadMap(char *filename) {
  std::ifstream ifs = std::ifstream(filename);
  nlohmann::json data = nlohmann::json::parse(ifs);
  
  for (auto tile : data["layers"][0]["data"]) {
    floor_layer.tiles.push_back(tile);
  }

  floor_layer.height = data["height"];
  floor_layer.width = data["width"];
}

extern "C" __declspec(dllexport) void __cdecl init(renderer_interface rendererInterface) {
  renderer = rendererInterface;
  // renderer.loadSpritesheet(filename, name, tileWidth, tileHeight, sheetWidth, sheetHeight, textureUnit)
  renderer.loadSpritesheet("assets/tiles.png", "spritesheet1", 16, 16, 4, 4, 0);

  loadMap("assets/testmap.tmj");
}

extern "C" __declspec(dllexport) void __cdecl update_and_render(
  game_state *game, 
  user_command command
) {
  float dt = 0.0166666666667;

  glm::vec2 movement = glm::vec2(0.0f);

  if (command.forward) {
    movement.y = 1.0f;
  }
  if (command.back) {
    movement.y = -1.0f;
  }
  if (command.left) {
    movement.x = -1.0f;
  }
  if (command.right) {
    movement.x = 1.0f;
  }

  if (movement != glm::vec2(0.0f)) {
    movement = glm::normalize(movement) * 400.0f * dt;
    
    game->playerX += movement.x;
    game->playerY += movement.y;  
  }

  renderer.setCameraPos((int)game->playerX, (int)game->playerY);

  for (int y = 0; y < floor_layer.height; y++) {
    for (int x = 0; x < floor_layer.width; x++) {
      int tile = floor_layer.tiles[y * floor_layer.width + x];
      if (tile != 0) {
        renderer.drawImage(x * 32.0, (floor_layer.height - y) * 32.0, 32.0, 32.0, "spritesheet1", tile - 1);
      }
    }
  }
  renderer.drawImage(game->playerX, game->playerY, 32.0, 32.0, "spritesheet1", 2);
}
