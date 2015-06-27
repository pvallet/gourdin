#pragma once
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <vector>
#include "camera.h"
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

	virtual void calculateFrustum(const Camera* camera) = 0; // Returns true if the chunk is to be displayed
	virtual void draw() const = 0;
	virtual float getHeight(float x, float y) const = 0;
	
	// To join the chunk with neighbours ones. Used with the perlin version.
	virtual Constraint getConstraint(sf::Vector2i fromChunkPos) const {Constraint c; c.type = NONE; return c;}

	inline GLuint getVBOIndex() const {return vbo;}
	inline GLuint getIBOIndex() const {return ibo;}
	inline sf::Vector2i getChunkPos() const {return chunkPos;}

	inline bool isVisible() const {return visible;}

protected:
	GLuint vbo;
	GLuint ibo;

	sf::Vector2i chunkPos;

	bool visible;
};

