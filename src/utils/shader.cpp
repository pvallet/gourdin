#include "shader.h"
#include "camera.h"

#include <cassert>

#ifdef __linux__
	#include <signal.h>
	#include <unistd.h>
#endif

Shader::Shader() :
	_vertexID(0),
	_fragmentID(0),
	_programID(0),
	_vertexSource(),
	_fragmentSource() {
}


Shader::Shader(Shader const &shader) {
  _vertexSource = shader._vertexSource;
  _fragmentSource = shader._fragmentSource;

  load();
}


Shader::Shader(std::string vertexSource, std::string fragmentSource) :
	_vertexID(0),
	_fragmentID(0),
	_programID(0),
	_vertexSource(vertexSource),
	_fragmentSource(fragmentSource) {
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

    std::cerr << "Error in shader linking: " << error << std::endl;

    // delete[] error;
   	glDeleteProgram(_programID);

    return false;
  }
}


bool Shader::compileShader(GLuint &shader, GLenum type, std::string const &source) {

  shader = glCreateShader(type);

  if(shader == 0) {
    std::cerr << "Error: There is no (" << type << ") shader type" << std::endl;
    return false;
  }

  std::ifstream file(source.c_str());

  if(!file) {
    std::cerr << "Error: File: " << source << " cannot be found" << std::endl;
		glDeleteShader(shader);

    return false;
  }

  std::string line;
  std::string sourceCode;

  while(getline(file, line))
    sourceCode += line + '\n';

  file.close();

  const GLchar* str = sourceCode.c_str();

	glShaderSource(shader, 1, &str, 0);

  glCompileShader(shader);

  GLint compilationError(0);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compilationError);

  if(compilationError != GL_TRUE) {
    GLint errorLength(0);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errorLength);

    char *error = new char[errorLength + 1];

    glGetShaderInfoLog(shader, errorLength, &errorLength, error);
    error[errorLength] = '\0';

    std::cerr << "Error in shader compilation: " << error << std::endl;

    // delete[] error;
		glDeleteShader(shader);

#ifdef __linux__
		kill(getpid(),SIGINT);
#endif

    return false;
  }

	else
    return true;
}
