#pragma once

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>

#include <vector>

#include "utils.h"

#define TEX_FACTOR 3.f // Number of times the texture is repeated per chunk

enum ConstraintType {XN, XP, YN, YP, NONE};

typedef struct Constraint Constraint;
struct Constraint {
	ConstraintType type;
	std::vector<sf::Vector3f> vertices;
};

class Chunk {
public:
	Chunk(sf::Vector2i chunkPosition);
	virtual ~Chunk();

	virtual void draw() const = 0;
	virtual float getHeight(float x, float y) const = 0;

	// Set visible to false if there is no need to display the chunk
	virtual void computeCulling();

	// To join the chunk with neighbours ones. Used with the perlin version.
	virtual Constraint getConstraint(sf::Vector2i fromChunkPos) const {Constraint c; c.type = NONE; return c;}

	inline GLuint getVBOIndex() const {return _vbo;}
	inline GLuint getIBOIndex() const {return _ibo;}
	inline sf::Vector2i getChunkPos() const {return _chunkPos;}

	inline bool isVisible() const {return _visible;}

protected:
	// 1 if they are all outside, -1 inside, else 0
	int compareToPoints(sf::Vector3f cam, sf::Vector3f vec, sf::Vector3f* points) const;
	GLuint _vbo;
	GLuint _ibo;

	sf::Vector2i _chunkPos;

	sf::Vector3f _corners[4]; // For frustum culling

	bool _visible;
};
