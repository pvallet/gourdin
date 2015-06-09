#pragma once
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <vector>
#include "camera.h"

#define CHUNK_SIZE 256.

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

	virtual bool calculateFrustum(const Camera* camera) = 0; // Returns true if the chunk is to be displayed
	virtual void draw() const = 0;
	virtual float getHeight(float x, float y) const = 0;
	
	virtual Constraint getConstraint(sf::Vector2i fromChunkPos) const = 0;

	inline GLuint getVBOIndex() const {return vbo;}
	inline GLuint getIBOIndex() const {return ibo;}
	inline sf::Vector2i getChunkPos() const {return chunkPos;}

	inline bool alreadyDisplayed() const {return displayed;}

protected:
	GLuint vbo;
	GLuint ibo;

	float* vertices;
	float* colors;
	float* coord;
	GLuint* indices;

	sf::Vector2i chunkPos;

	bool displayed;
};

