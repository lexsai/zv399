#include "game.h"
#include "map.h"

#include <glm/glm.hpp>

#include <fstream>
#include <iostream>

static renderer_interface renderer; 
static map_state map;

extern "C" __declspec(dllexport) void __cdecl init(renderer_interface rendererInterface) {
  renderer = rendererInterface;
  // renderer.loadSpritesheet(filename, name, tileWidth, tileHeight, sheetWidth, sheetHeight, textureUnit)
  renderer.loadSpritesheet("assets/tiles.png", "spritesheet1", 16, 16, 4, 4, 0);
  renderer.loadSpritesheet("assets/teto.png", "player", 16, 32, 4, 1, 2);
  renderer.loadSpritesheet("assets/bg3.jpg", "brickwall", 1280, 832, 1, 1, 1);

  loadMap(&map, "assets/testmap.tmj");
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
    movement = glm::normalize(movement) * 150.0f * dt;

    if (doesCollide(&map, game->playerX + movement.x, game->playerY)) {
      movement.x = 0.0f;
    }
    if (doesCollide(&map, game->playerX, game->playerY + movement.y)) {
      movement.y = 0.0f;
    }

    game->playerX = game->playerX + movement.x;
    game->playerY = game->playerY + movement.y;
  }
  std::cout << game->playerX << ", " << game->playerY << std::endl;

  renderer.setCameraPos(game->playerX, game->playerY);
 
  renderer.drawBackground(
    game->playerX - 240.0f, game->playerY - 144.0f, 
    481.0f, 481.0f, 
    "brickwall", 
    4, game->playerX + game->gameTime * 15, game->playerY + game->gameTime * 15
  );

  for (int y = 0; y < map.floorLayer.height; y++) {
    for (int x = 0; x < map.floorLayer.width; x++) {
      int tile = map.floorLayer.tiles[y * map.floorLayer.width + x];
      if (tile != 0) {
        renderer.drawImage(
          x * 16.0, (map.floorLayer.height - y - 1) * 16.0, 
          16.0, 16.0, 
          "spritesheet1", tile - 1
        );
      }
    }
  }

  renderer.drawImage(game->playerX, game->playerY, 16.0, 32.0, "player", 0);

  for (int y = 0; y < map.ceilingLayer.height; y++) {
    for (int x = 0; x < map.ceilingLayer.width; x++) {
      int tile = map.ceilingLayer.tiles[y * map.ceilingLayer.width + x];
      if (tile != 0) {
        renderer.drawImage(
          x * 16.0, (map.ceilingLayer.height - y - 1) * 16.0, 
          16.0, 16.0, 
          "spritesheet1", tile - 1);
      }
    }
  }
}
