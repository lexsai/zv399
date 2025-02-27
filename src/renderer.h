// struct render_command {
  
// };

void rendererInit();
void drawImage(float x, float y, float w, float h, char* textureName, int spriteIndex);
void drawBackground(float x, float y, float w, float h, char *textureName, float textureLoop, float offsetX, float offsetY);
void loadSpritesheet(
  char *filename, 
  char *textureName, 
  int tileWidth, 
  int tileHeight, 
  int sheetWidth, 
  int sheetHeight, 
  unsigned int textureUnit
);
void fillScreen(float r, float g, float b, float a);
void setCameraPos(float x, float y);
