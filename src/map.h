#ifndef MAP_H
#define MAP_H

#include <vector>

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

struct map_state {
  map_layer floorLayer;
  map_layer ceilingLayer;
  std::vector<collision_box> collisionBoxes;
};


bool inBox(float x, float y, collision_box box);
bool doesCollide(map_state *map, float x, float y);
void loadMap(map_state *map, char *filename);

#endif
