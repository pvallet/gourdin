#pragma once

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include "chunk.h"

class Ocean : public Chunk {
public:
	Ocean(size_t x, size_t y, GLuint tex);

	virtual void draw() const;

	virtual float getHeight(float x, float y) const {return 0.f;};

private:
	int compareToCorners(sf::Vector3f cam, sf::Vector3f vec) const;

	float _vertices[12];
	float _normals[12];
	float _coord[8];
	GLuint _indices[4];

	GLuint _tex;
};
