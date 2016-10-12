#pragma once

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <string>
#include <fstream>


class Shader {
public:

  Shader();
  Shader(Shader const &shader);
  Shader(std::string vertexSource, std::string fragmentSource);
  ~Shader();

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