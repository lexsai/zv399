#include "game.h"

#include "glm/glm.hpp"

static renderer_interface renderer; 

extern "C" __declspec(dllexport) void __cdecl init(renderer_interface rendererInterface) {
  renderer = rendererInterface;

  renderer.loadTexture("assets/transparentplayer.png", "player", 0);
  renderer.loadTexture("assets/bg2.jpg", "bg2", 1);
  renderer.loadTexture("assets/bg3.jpg", "bg3", 2);
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

  renderer.drawImage(0.0, 0.0, 1280.0 * 4, 720.0 * 4, "bg2");
  if (game->playerY <= 100.0) {
    renderer.drawImage(100.0, 100.0, 300.0, 300.0, "bg3");
  }
  renderer.drawImage(game->playerX, game->playerY, 50.0, 50.0, "player");
  if (game->playerY > 100.0) {
    renderer.drawImage(100.0, 100.0, 300.0, 300.0, "bg3");
  }
}
