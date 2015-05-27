#include "chunk.h"

Chunk::Chunk(sf::Vector2i chunkPosition) :
	chunkPos(chunkPosition) {
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);
}

Chunk::~Chunk() {
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
}

