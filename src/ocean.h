#pragma once

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include "chunk.h"

class Ocean : public Chunk {
public:
	Ocean(sf::Vector2i chunkPosition, GLuint _tex);
	~Ocean() {}

	virtual void calculateFrustum(const Camera* camera);

	virtual void draw() const;

	virtual float getHeight(float x, float y) const {return 0.f;};

private:
	int compareToCorners(sf::Vector3f cam, sf::Vector3f vec) const;

	float vertices[12];
	float normals[12];
	float coord[8];
	GLuint indices[4];

	GLuint tex;
};
