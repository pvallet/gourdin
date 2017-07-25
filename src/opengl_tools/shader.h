#pragma once

#include "glObjectInterface.h"
#include "opengl.h"
#include <string>

class Shader : public GLObjectInterface {
public:

  Shader ();
  Shader (Shader&& other) noexcept;
  ~Shader();

  bool load(std::string vertexSource, std::string fragmentSource);

  inline GLint getUniformLocation(const GLchar* name) const {return glGetUniformLocation(_programID, name);}

  inline void bind() const {glUseProgram(_programID);}

  static void unbind() {glUseProgram(0);}

private:
  bool compileShader(GLuint &shader, GLenum type, std::string const &sourceFile);

  GLuint _vertexID;
  GLuint _fragmentID;
  GLuint _programID;

  std::string _vertexSource;
  std::string _fragmentSource;
};
