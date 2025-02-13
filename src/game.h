#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include <stdbool.h>

struct game_state {
  uint32_t *display_backbuffer;
  int playerX;
  int playerY;
  int playerWidth;
};

struct user_command {
  bool forward;
  bool back;
  bool left;
  bool right;
};

#endif