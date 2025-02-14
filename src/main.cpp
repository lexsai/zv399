#include "game.h"
#include "renderer.h"

#include <Windows.h>

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <iostream>

typedef void (__cdecl *UPDATEPROC)(
  struct game_state *game, 
  struct user_command userCommand
); 

typedef void (__cdecl *INITPROC)(
  PFNGLCLEARCOLORPROC clearColorPointer
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

static void loadGameLibrary() {
  CopyFile("game.dll", "game-temp.dll", FALSE);
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
  gameLibrary.init(fillScreen);
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

    default: break;
  }
}

void gameInit() {
  gameState.playerWidth = 10;
  gameState.playerX = 100;
  gameState.playerY = 100;
  gameState.display_backbuffer = (uint32_t *)calloc(1280 * 720, sizeof(uint32_t));
}

int main(int argc, char** argv) {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    return -1;
  }

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
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

  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    std::cout << "failed to load GL functions" << std::endl;
    return -1;
  }

  gameInit();
  rendererInit();

  int counter = 0;
  while (true) {
    if (counter == 0) {
      if (gameLibrary.loaded) {
        freeGameLibrary();
      }
      loadGameLibrary();
      counter = 120;
    }
    counter--;

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
    
    if (gameLibrary.loaded) {
      gameLibrary.updateAndRender(&gameState, userCommand);
    }

    SDL_GL_SwapWindow(window);

    // int pitch;
    // char *pix;
    // SDL_LockTexture(texture, NULL, &pix, &pitch);
    // memcpy(pix, gameState.display_backbuffer, 1280 * 720 * 4);
    // SDL_UnlockTexture(texture);

    // SDL_RenderTexture(renderer, texture, NULL, NULL);
    // SDL_RenderPresent(renderer);
  }

  return 0;
}
