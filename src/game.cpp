#include <glad/glad.h>

#include "game.h"

static PFNGLCLEARCOLORPROC clearColor;

extern "C" __declspec(dllexport) void __cdecl init(PFNGLCLEARCOLORPROC clearColorPointer) {
  clearColor = clearColorPointer;
}

extern "C" __declspec(dllexport) void __cdecl update_and_render(
  struct game_state *game, 
  struct user_command command
) {
  game->playerWidth = 20;
  int pitch = 1280;
  for (int y = 0; y < 720; y++) {
    for (int x = 0; x < 1280; x++) {
      *(game->display_backbuffer + y * pitch + x) = 0xff0000ff;
    }
  }

  if (command.forward) {
    game->playerY -= 1;
  }
  if (command.back) {
    game->playerY += 1;
  }
  if (command.left) {
    game->playerX -= 1;
  }
  if (command.right) {
    game->playerX += 1;
  }

  clearColor(0.3f, 0.3f, 1.0f, 1.0f);

  int halfPlayerWidth = game->playerWidth / 2;
  for (int y = game->playerY - halfPlayerWidth; y < game->playerY + halfPlayerWidth; y++) {
    for (int x = game->playerX - halfPlayerWidth; x < game->playerX + halfPlayerWidth; x++) {
      if (y < 0 || y >= 720) {
        continue; 
      };
      if (x < 0 || x >= 1280) { 
        continue; 
      };  

      *(game->display_backbuffer + y * pitch + x) = 0xffffffff;
    }
  }
}
