#pragma once

#include "basicGLObjects.h"
#include "texture.h"

#include <array>

// Big plate to draw under the map
class Ocean {
public:
	Ocean(float oversizeFactor);

	void draw() const;

	void setTexture(const Texture* texture) {_texture = texture;}

private:
	std::array<float,12> _vertices;
	std::array<float,12> _normals;
	std::array<float,8>  _coord;
	std::array<GLuint,4> _indices;

	VertexArrayObject _vao;
	VertexBufferObject _vbo;
	IndexBufferObject _ibo;

	const Texture* _texture;
};
