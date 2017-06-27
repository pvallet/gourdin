#pragma once

#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>
#include <string>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Shader {
public:

  Shader();
  Shader(Shader const &shader);
  Shader(std::string vertexSource, std::string fragmentSource);
  virtual ~Shader();

  Shader& operator=(Shader const &shader);

  bool load();
  bool compileShader(GLuint &shader, GLenum type, std::string const &sourceFile);
  inline GLuint getProgramID() const {return _programID;}

private:

  GLuint _vertexID;
  GLuint _fragmentID;
  GLuint _programID;

  std::string _vertexSource;
  std::string _fragmentSource;
};
