#pragma once

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include "chunk.h"

class Ocean : public Chunk {
public:
	Ocean(size_t x, size_t y, GLuint tex);
	virtual ~Ocean() {}

	virtual void draw() const;

	virtual float getHeight(float x, float y) const {return 0.f;};

private:
	int compareToCorners(sf::Vector3f cam, sf::Vector3f vec) const;

	std::array<float,12> _vertices;
	std::array<float,12> _normals;
	std::array<float,8>  _coord;
	std::array<GLuint,4> _indices;

	GLuint _tex;
};
