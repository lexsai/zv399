#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include <stdbool.h>

struct game_state {
  float playerX;
  float playerY;
};

struct user_command {
  bool forward;
  bool back;
  bool left;
  bool right;
};

typedef void (__cdecl *DRAWIMAGEPROC)(
  float x, float y,
  float w, float h,
  char* textureName,
  int spriteIndex
); 

typedef void (__cdecl *LOADSPRITESHEETPROC)(
  char *filename, 
  char *textureName, 
  int tileWidth, 
  int tileHeight, 
  int sheetWidth, 
  int sheetHeight, 
  unsigned int textureUnit
); 

typedef void (__cdecl *SETCAMERAPOSPROC)(
  float x, float y
); 

struct renderer_interface {
  DRAWIMAGEPROC drawImage;
  LOADSPRITESHEETPROC loadSpritesheet;
  SETCAMERAPOSPROC setCameraPos;
};

#endif
