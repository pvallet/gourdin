#include "shader.h"

#include "utils.h"

#include <SDL_log.h>
#include <cassert>

Shader::Shader() :
	_vertexID(0),
	_fragmentID(0),
	_programID(0) {}


Shader::Shader(Shader const &shader) {
  _vertexSource = shader._vertexSource;
  _fragmentSource = shader._fragmentSource;

  load();
}

Shader::~Shader() {
	glDeleteShader(_vertexID);
	glDeleteShader(_fragmentID);
  glDeleteProgram(_programID);
}


Shader& Shader::operator=(Shader const &shader) {
  _vertexSource = shader._vertexSource;
  _fragmentSource = shader._fragmentSource;

  load();

  return *this;
}

bool Shader::load(std::string vertexSource, std::string fragmentSource) {
	_vertexSource = vertexSource;
	_fragmentSource = fragmentSource;

	return load();
}

bool Shader::load() {
  if(glIsShader(_vertexID) == GL_TRUE)
  	glDeleteShader(_vertexID);

  if(glIsShader(_fragmentID) == GL_TRUE)
    glDeleteShader(_fragmentID);

  if(glIsProgram(_programID) == GL_TRUE)
    glDeleteProgram(_programID);


  if(!compileShader(_vertexID, GL_VERTEX_SHADER, _vertexSource))
    return false;

  if(!compileShader(_fragmentID, GL_FRAGMENT_SHADER, _fragmentSource))
    return false;

  _programID = glCreateProgram();

  glAttachShader(_programID, _vertexID);
  glAttachShader(_programID, _fragmentID);

  glLinkProgram(_programID);

  GLint errorLink(0);
  glGetProgramiv(_programID, GL_LINK_STATUS, &errorLink);

  if(errorLink != GL_TRUE) {
    GLint sizeError(0);
    glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &sizeError);

    char *error = new char[sizeError + 1];

    glGetShaderInfoLog(_programID, sizeError, &sizeError, error);
    error[sizeError] = '\0';

    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error in shader linking: %s", error);

    delete[] error;
   	glDeleteProgram(_programID);

    return false;
  }

	return true;
}


bool Shader::compileShader(GLuint &shader, GLenum type, std::string const &source) {

  shader = glCreateShader(type);

  if(shader == 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error: There is no (%d) shader type", type);
    return false;
  }

  std::string sourceCode = ut::textFileToString(source);

  const GLchar* str = sourceCode.c_str();

#ifdef __ANDROID__
	const char *sources[2] = { "#version 300 es\n", str };
#else
	const char *sources[2] = { "#version 330\n", str };
#endif

	glShaderSource(shader, 2, sources, 0);

  glCompileShader(shader);

  GLint compilationError(0);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compilationError);

  if(compilationError != GL_TRUE) {
    GLint errorLength(0);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errorLength);

    char *error = new char[errorLength + 1];

    glGetShaderInfoLog(shader, errorLength, &errorLength, error);
    error[errorLength] = '\0';

    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error in shader compilation: %s", error);

    delete[] error;
		glDeleteShader(shader);

		assert(false);

    return false;
  }

	else
    return true;
}
