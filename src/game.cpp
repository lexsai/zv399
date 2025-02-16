#include "game.h"

#include "glm/glm.hpp"

static renderer_interface renderer; 

extern "C" __declspec(dllexport) void __cdecl init(renderer_interface rendererInterface) {
  renderer = rendererInterface;

  renderer.loadTexture("assets/transparentplayer.png", "player", 0);
  renderer.loadTexture("assets/bg2.jpg", "bg2", 1);
  renderer.loadTexture("assets/bg3.jpg", "bg3", 2);

  renderer.loadTexture("assets/ceilingbar.png", "ceiling", 3);
  renderer.loadTexture("assets/floor.png", "floor", 4);
  renderer.loadTexture("assets/wall.png", "wall", 5);
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

  // floor
  renderer.drawImage(0.0, 0.0, 2048, 2048, "floor");
  // player
  renderer.drawImage(game->playerX, game->playerY, 50.0, 50.0, "player");

  for (int i = 0; i < 4; i++) {
    renderer.drawImage(i * 512, 256, 100, 2048 - 256, "ceiling");
  }
  // wall
  for (int i = 0; i < 8; i++) {
    renderer.drawImage(i * 256, 0.0, 256, 256, "wall");
  }
}
