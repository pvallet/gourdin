#pragma once

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <string>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera;

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

  void sendModelMatrix(Camera *cam, const glm::mat4& model) const;

private:

  GLuint _vertexID;
  GLuint _fragmentID;
  GLuint _programID;

  GLint _matrixID; // for the modelview and projection matrix multiplication

  std::string _vertexSource;
  std::string _fragmentSource;
};
