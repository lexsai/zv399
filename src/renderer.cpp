#include "renderer.h"

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

static unsigned int shaderProgram;
static unsigned int VAO;
static float cameraX;
static float cameraY;
static std::unordered_map<std::string, unsigned int> textureMap;

void loadTexture(char *filename, char *textureName, unsigned int textureUnit) {
  unsigned int texture;
  glGenTextures(1, &texture);

  glActiveTexture(GL_TEXTURE0 + textureUnit);
  glBindTexture(GL_TEXTURE_2D, texture); 

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
  if (data)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
    std::cout << "Failed to load texture: " << filename << " in texture unit " << textureUnit << std::endl;
  }
  stbi_image_free(data);;

  textureMap[textureName] = textureUnit;
}

static std::string readFileToString(char *filename) {
  std::ostringstream fileContentStream;
  try {
    std::ifstream ifs = std::ifstream(filename);
    fileContentStream << ifs.rdbuf();
    ifs.close();
  }
  catch (std::ifstream::failure& e) {
    std::cout << "failed to read shader file contents: " << e.what() << std::endl;
  }
  return fileContentStream.str();
}

static unsigned int createShaderProgram(char *vertFile, char *fragFile) {
  std::string vertexCode = readFileToString(vertFile);
  std::string fragmentCode = readFileToString(fragFile);
  const char *vertexShaderSource = vertexCode.c_str();
  const char *fragmentShaderSource = fragmentCode.c_str();

  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, 0);
  glCompileShader(vertexShader);
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(vertexShader, 512, 0, infoLog);
    std::cout << "failed to compile vertex" << std::endl;
  }
  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, 0);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(fragmentShader, 512, 0, infoLog);
    std::cout << "failed to compile frag" << std::endl;
  }
  unsigned int newShaderProgram = glCreateProgram();
  glAttachShader(newShaderProgram, vertexShader);
  glAttachShader(newShaderProgram, fragmentShader);
  glLinkProgram(newShaderProgram);
  glGetProgramiv(newShaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(newShaderProgram, 512, 0, infoLog);
    std::cout << "failed to link" << std::endl;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  return newShaderProgram;
}

void rendererInit() {
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    std::cout << "failed to load GL functions" << std::endl;
  }

  shaderProgram = createShaderProgram("assets/shaders/vert.glsl", "assets/shaders/frag.glsl");

  float vertices[] = {
    // positions      // texture coords
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top right
    1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right
    0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top left
    1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom left
    0.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
  };

  unsigned int VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);  

  glBindBuffer(GL_ARRAY_BUFFER, 0); 
  glBindVertexArray(0); 

  glUniform1i(glGetUniformLocation(shaderProgram, "currentTextureUnit"), 0);
}

void drawImage(float x, float y, float w, float h, char *textureName) {
  glUseProgram(shaderProgram);

  glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x - 0.5, y - 0.5, 0.0f));
  model = glm::scale(model, glm::vec3(w, h, 1.0f));
  glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
  glm::mat4 projection = glm::ortho(
    cameraX - 640, cameraX + 640, 
    cameraY - 360, cameraY + 360, 
    0.1f, 10.0f
  );

  glm::mat4 mvp = projection * view * model;

  int mvpLoc = glGetUniformLocation(shaderProgram, "mvp");
  glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

  glUniform1i(glGetUniformLocation(shaderProgram, "currentTextureUnit"), textureMap[textureName]);

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);  
}

void fillScreen(float r, float g, float b, float a) {
  glClearColor(r, g, b, a);
  glClear(GL_COLOR_BUFFER_BIT);
}

void setCameraPos(float x, float y) {
  cameraX = x;
  cameraY = y;
}
