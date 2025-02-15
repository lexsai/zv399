#include "game.h"

#include "glm/glm.hpp"

static DRAWTRIANGLEPROC drawTriangle;

extern "C" __declspec(dllexport) void __cdecl init(DRAWTRIANGLEPROC drawTrianglePointer) {
  drawTriangle = drawTrianglePointer;
}

extern "C" __declspec(dllexport) void __cdecl update_and_render(
  game_state *game, 
  user_command command
) {
  float dt = 0.015;

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
    movement = glm::normalize(movement) * 1.0f * dt;
    
    game->playerX += movement.x;
    game->playerY += movement.y;
  }

  drawTriangle(game->playerX, game->playerY, 0.0f, 0.0f, 0.0f, 1.0f);
}
