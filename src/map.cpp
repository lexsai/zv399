#include "map.h"

#include <fstream>
#include <nlohmann/json.hpp>

bool inBox(float x, float y, collision_box box) {
  return x > box.beginX && x < box.endX && y > box.beginY && y < box.endY;
}

bool doesCollide(map_state *map, float x, float y) {
  bool collides = false;
  for (collision_box box : map->collisionBoxes) {
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

void loadMap(map_state *map, char *filename) {
  std::ifstream ifs = std::ifstream(filename);
  nlohmann::json data = nlohmann::json::parse(ifs);
  
  for (auto layer: data["layers"]) {
    if (layer["name"] == "floor") {  
      for (int tile : layer["data"]) {
        map->floorLayer.tiles.push_back(tile);
      }
      map->floorLayer.height = layer["height"];
      map->floorLayer.width = layer["width"];
      
    }
    if (layer["name"] == "ceiling") {  
      for (int tile : layer["data"]) {
        map->ceilingLayer.tiles.push_back(tile);
      }
      map->ceilingLayer.height = layer["height"];
      map->ceilingLayer.width = layer["width"];
    }
    if (layer["name"] == "collisions") {  
      for (auto wallData : layer["objects"]) {
        collision_box collisionBox = {};
        collisionBox.beginX = wallData["x"];
        collisionBox.beginY = 320 - (int)wallData["y"] - (int)wallData["height"];
        collisionBox.endX = collisionBox.beginX + (int)wallData["width"];
        collisionBox.endY = collisionBox.beginY + (int)wallData["height"]; 

        map->collisionBoxes.push_back(collisionBox);
      }
    }
  }
 
}
