#pragma once

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <array>

// Big plate to draw under the map
class Ocean {
public:
	Ocean(float oversizeFactor);
	~Ocean();
	Ocean(Ocean const&)          = delete;
	void operator=(Ocean const&) = delete;

	void draw() const;

	void setTexIndex(GLuint tex) {_tex = tex;}

private:
	std::array<float,12> _vertices;
	std::array<float,12> _normals;
	std::array<float,8>  _coord;
	std::array<GLuint,4> _indices;

	GLuint _vao;
	GLuint _vbo;
	GLuint _ibo;
	GLuint _tex;
};
