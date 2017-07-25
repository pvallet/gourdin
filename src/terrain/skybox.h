#pragma once

#include <array>

#include "basicGLObjects.h"
#include "texArray.h"

class Skybox {
public:
	Skybox();

	void load(std::string filename);
	void draw() const;

private:
	float t;

	std::array<GLuint,36> _indices;
	std::array<float,72> _vertices;
	std::array<float,48> _coord;
	std::array<float,24> _layer;

	VertexArrayObject _vao;
	VertexBufferObject _vbo;
	IndexBufferObject _ibo;

	TextureArray _texArray;
};
