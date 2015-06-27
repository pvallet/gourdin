#include "shader.h"

Shader::Shader() : 
	vertexID(0),
	fragmentID(0),
	programID(0),
	vertexSource(),
	fragmentSource() {
}


Shader::Shader(Shader const &_shader) {
    vertexSource = _shader.vertexSource;
    fragmentSource = _shader.fragmentSource;

    load();
}


Shader::Shader(std::string _vertexSource, std::string _fragmentSource) : 
	vertexID(0),
	fragmentID(0),
	programID(0),
	vertexSource(_vertexSource),
	fragmentSource(_fragmentSource) {
}


Shader::~Shader() {
	glDeleteShader(vertexID);
    glDeleteShader(fragmentID);
    glDeleteProgram(programID);
}


Shader& Shader::operator=(Shader const &_shader) {
    vertexSource = _shader.vertexSource;
    fragmentSource = _shader.fragmentSource;

    load();

    return *this;
}


bool Shader::load() {
    if(glIsShader(vertexID) == GL_TRUE)
        glDeleteShader(vertexID);

    if(glIsShader(fragmentID) == GL_TRUE)
        glDeleteShader(fragmentID);

    if(glIsProgram(programID) == GL_TRUE)
        glDeleteProgram(programID);


    if(!compileShader(vertexID, GL_VERTEX_SHADER, vertexSource))
        return false;

    if(!compileShader(fragmentID, GL_FRAGMENT_SHADER, fragmentSource))
        return false;

    programID = glCreateProgram();

    glAttachShader(programID, vertexID);
    glAttachShader(programID, fragmentID);

    glBindAttribLocation(programID, 0, "in_Vertex");
    glBindAttribLocation(programID, 1, "in_Color");
    glBindAttribLocation(programID, 2, "in_TexCoord0");

    glLinkProgram(programID);

    GLint errorLink(0);
    glGetProgramiv(programID, GL_LINK_STATUS, &errorLink);

    if(errorLink != GL_TRUE) {

        GLint sizeError(0);
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &sizeError);

        char *error = new char[sizeError + 1];

        glGetShaderInfoLog(programID, sizeError, &sizeError, error);
        error[sizeError] = '\0';

        std::cout << error << std::endl;

        delete[] error;
        glDeleteProgram(programID);

        return false;
    }

    else
        return true;
}


bool Shader::compileShader(GLuint &shader, GLenum type, std::string const &source) {

    shader = glCreateShader(type);

    if(shader == 0) {
        std::cout << "Error: There is no (" << type << ") shader type" << std::endl;
        return false;
    }

    std::ifstream file(source.c_str());

    if(!file) {
        std::cout << "Error: File: " << source << " cannot be found" << std::endl;
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

        std::cout << error << std::endl;

        delete[] error;
        glDeleteShader(shader);

        return false;
    }

    else
        return true;
}

