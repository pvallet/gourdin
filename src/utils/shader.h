#pragma once

#include "opengl.h"
#include <iostream>
#include <string>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Shader {
public:

  Shader();
  Shader(Shader const &shader);
  virtual ~Shader();

  Shader& operator=(Shader const &shader);

  bool load(std::string vertexSource, std::string fragmentSource);
  inline GLuint getProgramID() const {return _programID;}

private:
  bool compileShader(GLuint &shader, GLenum type, std::string const &sourceFile);
  bool load();

  GLuint _vertexID;
  GLuint _fragmentID;
  GLuint _programID;

  std::string _vertexSource;
  std::string _fragmentSource;
};
