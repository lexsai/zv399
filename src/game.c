#include "game.h"


__declspec(dllexport) void __cdecl update_and_render(
  struct game_state *game, 
  struct user_command command
) {
  game->playerWidth = 20;
  int pitch = 1280;
  for (int y = 0; y < 720; y++) {
    for (int x = 0; x < 1280; x++) {
      *(game->display_backbuffer + y * pitch + x) = 0xffff0000;
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