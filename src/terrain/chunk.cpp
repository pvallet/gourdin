#include "chunk.h"

Chunk::Chunk(sf::Vector2i chunkPosition) :
	_chunkPos(chunkPosition),
	_visible(false) {
	glGenBuffers(1, &_vbo);
	glGenBuffers(1, &_ibo);
}

Chunk::~Chunk() {
	glDeleteBuffers(1, &_vbo);
	glDeleteBuffers(1, &_ibo);
}
