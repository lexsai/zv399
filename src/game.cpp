#include "game.h"

#include "glm/glm.hpp"

static IMAGEPROC drawImage;

// TODO(lexsai): move these callback functions to a struct!
extern "C" __declspec(dllexport) void __cdecl init(
  IMAGEPROC drawImagePointer
) {
  drawImage = drawImagePointer;
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
    movement = glm::normalize(movement) * 50.0f * dt;
    
    game->playerX += movement.x;
    game->playerY += movement.y;
  }

  drawImage(0.0, 0.0, 1280.0 * 4, 720.0 * 4, 1);
  drawImage(game->playerX, game->playerY, 50.0, 50.0, 0);
}
