#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include <stdbool.h>

struct game_state {
  float playerX;
  float playerY;
};

struct user_command {
  bool forward;
  bool back;
  bool left;
  bool right;
};

typedef void (__cdecl *IMAGEPROC)(
  float x, float y,
  float w, float h,
  int textureUnit
); 

#endif
