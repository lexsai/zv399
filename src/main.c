#include <stdio.h>

#include <Windows.h>
// #include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "game.h"

#if 0
const GLuint WIDTH = 800, HEIGHT = 600;
int main(int argc, char** argv) {
    // code without checking for errors
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window = SDL_CreateWindow(
        "[glad] GL with SDL",
        WIDTH, HEIGHT,
        SDL_WINDOW_OPENGL
    );

    SDL_GLContext context = SDL_GL_CreateContext(window);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
      SDL_Log("failed to load GL functions");
      return -1;
    }

    int exit = 0;
    while(!exit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_EVENT_QUIT:
                    exit = 1;
                    break;
            }
        }

        glClearColor(0.7f, 0.9f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(window);
        SDL_Delay(1);
    }

    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
#endif

typedef void (__cdecl *UPDATEPROC)(
  struct game_state *game, 
  struct user_command userCommand
); 

struct game_library {
  SDL_SharedObject *handle;
  UPDATEPROC updateAndRender;
  bool loaded;
};

static struct game_library gameLibrary;
static struct game_state gameState;
static struct user_command userCommand;
static bool globalRunning;

static void loadGameLibrary() {
  CopyFile("game.dll", "game-temp.dll", FALSE);
  gameLibrary.handle = SDL_LoadObject("game-temp.dll");
  if (!gameLibrary.handle) {
    SDL_Log("failed to load library");
    return;
  }

  gameLibrary.updateAndRender = (UPDATEPROC)SDL_LoadFunction(gameLibrary.handle, "update_and_render");
  if (!gameLibrary.updateAndRender) {
    SDL_Log("failed to find method");
    return;
  }

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
  gameState.display_backbuffer = calloc(1280 * 720, sizeof(uint32_t));
}

int main(int argc, char** argv) {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    return -1;
  }

  SDL_Window *Window = SDL_CreateWindow("zv399", 1280, 720, 0);
  SDL_Renderer *Renderer = SDL_CreateRenderer(Window, NULL);
  SDL_Texture *Texture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, 1280, 720);
  
  if (!Window || !Renderer || !Texture) {
    return -1;
  }

  gameInit();

  int counter = 0;
  globalRunning = true;
  while (globalRunning) {
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
  
    int pitch;
    char *pix;
    SDL_LockTexture(Texture, NULL, &pix, &pitch);
    memcpy(pix, gameState.display_backbuffer, 1280 * 720 * 4);
    SDL_UnlockTexture(Texture);

    SDL_RenderTexture(Renderer, Texture, NULL, NULL);
    SDL_RenderPresent(Renderer);
  }

  return 0;
}
