// struct render_command {
  
// };

void rendererInit();
void drawImage(float x, float y, float w, float h, char* textureName);
void loadTexture(char *filename, char *textureName, unsigned int textureUnit);
void fillScreen(float r, float g, float b, float a);
void setCameraPos(float x, float y);
