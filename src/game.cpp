#include "game.h"

static DRAWTRIANGLEPROC drawTriangle;

extern "C" __declspec(dllexport) void __cdecl init(DRAWTRIANGLEPROC drawTrianglePointer) {
  drawTriangle = drawTrianglePointer;
}

extern "C" __declspec(dllexport) void __cdecl update_and_render(
  struct game_state *game, 
  struct user_command command
) {
  if (command.forward) {
    game->playerY -= 0.01;
  }
  if (command.back) {
    game->playerY += 0.01;
  }
  if (command.left) {
    game->playerX -= 0.01;
  }
  if (command.right) {
    game->playerX += 0.01;
  }

  drawTriangle(game->playerX, -game->playerY);
}
