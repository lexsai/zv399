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
struct collision_box {
  int beginX;
  int beginY;
  int endX;
  int endY;
};

static map_layer floorLayer;
static map_layer ceilingLayer;

static std::vector<collision_box> collisionBoxes;

static bool inBox(float x, float y, collision_box box) {
  return x > box.beginX && x < box.endX && y > box.beginY && y < box.endY;
}

static bool doesCollide(float x, float y) {
  bool collides = false;
  for (collision_box box : collisionBoxes) {
    if (inBox(x, y, box)
        || inBox(x + 16, y, box)
        || inBox(x, y + 16, box)
        || inBox(x + 16, y + 16, box)
    ) {
      collides = true;
    }
  }
  return collides;
}

static void loadMap(char *filename) {
  std::ifstream ifs = std::ifstream(filename);
  nlohmann::json data = nlohmann::json::parse(ifs);
  
  for (auto layer: data["layers"]) {
    if (layer["name"] == "floor") {  
      for (int tile : layer["data"]) {
        floorLayer.tiles.push_back(tile);
      }
      floorLayer.height = layer["height"];
      floorLayer.width = layer["width"];
      
    }
    if (layer["name"] == "ceiling") {  
      for (int tile : layer["data"]) {
        ceilingLayer.tiles.push_back(tile);
      }
      ceilingLayer.height = layer["height"];
      ceilingLayer.width = layer["width"];
    }
    if (layer["name"] == "collisions") {  
      for (auto wallData : layer["objects"]) {
        collision_box collisionBox = {};
        collisionBox.beginX = wallData["x"];
        collisionBox.beginY = 320 - (int)wallData["y"] - (int)wallData["height"];
        collisionBox.endX = collisionBox.beginX + (int)wallData["width"];
        collisionBox.endY = collisionBox.beginY + (int)wallData["height"]; 

        collisionBoxes.push_back(collisionBox);
      }
    }
  }
 
}

extern "C" __declspec(dllexport) void __cdecl init(renderer_interface rendererInterface) {
  renderer = rendererInterface;
  // renderer.loadSpritesheet(filename, name, tileWidth, tileHeight, sheetWidth, sheetHeight, textureUnit)
  renderer.loadSpritesheet("assets/tiles.png", "spritesheet1", 16, 16, 4, 4, 0);
  renderer.loadSpritesheet("assets/bg3.jpg", "brickwall", 1280, 832, 1, 1, 1);

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
    movement = glm::normalize(movement) * 200.0f * dt;

    if (doesCollide(game->playerX + movement.x, game->playerY)) {
      movement.x = 0.0f;
    }
    if (doesCollide(game->playerX, game->playerY + movement.y)) {
      movement.y = 0.0f;
    }

    game->playerX = game->playerX + movement.x;
    game->playerY = game->playerY + movement.y;
  }

  renderer.setCameraPos(game->playerX, game->playerY);
 
  renderer.drawBackground(
    game->playerX - 240.0f, game->playerY - 144.0f, 
    481.0f, 481.0f, 
    "brickwall", 
    4, game->playerX / 100.0f + game->gameTime / 20.0f, game->playerY / 100.0f + game->gameTime / 20.0f);

  for (int y = 0; y < floorLayer.height; y++) {
    for (int x = 0; x < floorLayer.width; x++) {
      int tile = floorLayer.tiles[y * floorLayer.width + x];
      if (tile != 0) {
        renderer.drawImage(
          x * 16.0, (floorLayer.height - y - 1) * 16.0, 
          16.0, 16.0, 
          "spritesheet1", tile - 1
        );
      }
    }
  }

  renderer.drawImage(game->playerX, game->playerY, 16.0, 16.0, "spritesheet1", 8);
  renderer.drawImage(game->playerX, game->playerY + 16, 16.0, 16.0, "spritesheet1", 4);

  for (int y = 0; y < ceilingLayer.height; y++) {
    for (int x = 0; x < ceilingLayer.width; x++) {
      int tile = ceilingLayer.tiles[y * ceilingLayer.width + x];
      if (tile != 0) {
        renderer.drawImage(
          x * 16.0, (ceilingLayer.height - y - 1) * 16.0, 
          16.0, 16.0, 
          "spritesheet1", tile - 1);
      }
    }
  }
}
