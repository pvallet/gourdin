#pragma once
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <vector>
#include "camera.h"

#define CHUNK_SIZE 256.

class Chunk {
public:
	Chunk(sf::Vector2i chunkPosition);
	virtual ~Chunk();

	virtual bool calculateFrustum(const Camera* camera) = 0; // Returns true if the chunk is to be displayed
	virtual void draw() const = 0;
	virtual float getHeight(float x, float y) const = 0;
	//static void merge(std::vector<Chunk>)

	inline GLuint getVBOIndex() const {return vbo;}
	inline GLuint getIBOIndex() const {return ibo;}
	inline sf::Vector2i getChunkPos() const {return chunkPos;}

protected:
	GLuint vbo;
	GLuint ibo;

	float* vertices;
	float* colors;
	GLuint* indices;

	sf::Vector2i chunkPos;
};

