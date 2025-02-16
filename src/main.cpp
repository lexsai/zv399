#include "game.h"
#include "renderer.h"

#include <Windows.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <iostream>

typedef void (__cdecl *UPDATEPROC)(
  struct game_state *game, 
  struct user_command userCommand
); 

typedef void (__cdecl *INITPROC)(
  renderer_interface rendererInterface
); 

struct game_library {
  SDL_SharedObject *handle;
  UPDATEPROC updateAndRender;
  INITPROC init;

  bool loaded;
};

static struct game_library gameLibrary;
static struct game_state gameState;
static struct user_command userCommand;
static int lagTime;
static int prevTime;

static void loadGameLibrary() {
  if (!CopyFileA("game.dll", "game-temp.dll", FALSE)) {
    std::cout << "failed to copy dll" << std::endl;
    return;
  }
  gameLibrary.handle = SDL_LoadObject("game-temp.dll");
  if (!gameLibrary.handle) {
    std::cout << "failed to load library" << std::endl;
    return;
  }

  gameLibrary.updateAndRender = (UPDATEPROC)SDL_LoadFunction(gameLibrary.handle, "update_and_render");
  if (!gameLibrary.updateAndRender) {
    std::cout << "failed to find method 'update_and_render'" << std::endl;
    return;
  }

  gameLibrary.init = (INITPROC)SDL_LoadFunction(gameLibrary.handle, "init");
  if (!gameLibrary.init) {
    std::cout << "failed to find method 'init'" << std::endl;
    return;
  }

  renderer_interface rendererInterface = {};
  rendererInterface.drawImage = drawImage;
  rendererInterface.loadTexture = loadTexture;
  
  gameLibrary.init(rendererInterface);
  gameLibrary.loaded = true;
}

static void freeGameLibrary() {
  SDL_UnloadObject(gameLibrary.handle);
  gameLibrary.handle = 0;
  gameLibrary.updateAndRender = 0;
  gameLibrary.loaded = false;
}

static void receiveInput(SDL_Scancode keyCode, bool isDown) {
  switch (keyCode) {
    case SDL_SCANCODE_W: {
      userCommand.forward = isDown;
    } break;

    case SDL_SCANCODE_S: {
      userCommand.back = isDown;
    } break;
    
    case SDL_SCANCODE_A: {
      userCommand.left = isDown;
    } break;

    case SDL_SCANCODE_D: {
      userCommand.right = isDown;
    } break;

    case SDL_SCANCODE_R: {
      if (!isDown) {
        if (gameLibrary.loaded) {
          freeGameLibrary();
        }
        loadGameLibrary();
        break;
      }
    }

    default: break;
  }
}

void gameInit() {
  gameState.playerX = 5.0f;
  gameState.playerY = 5.0f;
}

int main(int argc, char** argv) {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    return -1;
  }
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  SDL_Window *window = SDL_CreateWindow("zv399", 1280, 720, SDL_WINDOW_OPENGL);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
  SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, 1280, 720);  
  SDL_GLContext context = SDL_GL_CreateContext(window);

  if (!window || !renderer || !texture || !context) {
    return -1;
  }
  
  
  gameInit();
  rendererInit();
  loadGameLibrary();
  
  while (true) {
    if (lagTime > 0) {      
      SDL_Event event; 
      while (SDL_PollEvent(&event)) {
        switch (event.type) {
          case SDL_EVENT_QUIT: {
            return -1;
          } break;

          case SDL_EVENT_KEY_DOWN: {
            receiveInput(event.key.scancode, true);
          } break;
          
          case SDL_EVENT_KEY_UP: {
            receiveInput(event.key.scancode, false);
          } break;

          default: break;
        }
      }

      setCameraPos(gameState.playerX, gameState.playerY);

      fillScreen(0.3f, 0.5f, 1.0f, 1.0f);

      if (gameLibrary.loaded) {
        gameLibrary.updateAndRender(&gameState, userCommand);
      }
      SDL_GL_SwapWindow(window);
    }
    int nowTime = SDL_GetTicks();
    lagTime += nowTime - prevTime;
    prevTime = nowTime;
  }

  return 0;
}
