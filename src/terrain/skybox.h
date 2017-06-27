#pragma once

#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include <array>

#include "texArray.h"

class Skybox {
public:
	Skybox();
	~Skybox();
	Skybox(Skybox const&)         = delete;
	void operator=(Skybox const&) = delete;

	void load(std::string filename);
	void draw() const;

private:
	float t;

	std::array<GLuint,36> _indices;
	std::array<float,72> _vertices;
	std::array<float,48> _coord;
	std::array<float,24> _layer;

	GLuint _vao;
	GLuint _vbo;
	GLuint _ibo;

	TextureArray _texArray;
};
