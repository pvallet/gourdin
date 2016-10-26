#include "ocean.h"
#include "vecUtils.h"

#define BUFFER_OFFSET(a) ((char*)NULL + (a))

Ocean::Ocean(sf::Vector2i chunkPosition, GLuint tex) :
	Chunk(chunkPosition),
	_vertices{0, 0, 0,
			CHUNK_SIZE, 0, 0,
			 0, CHUNK_SIZE, 0,
			 CHUNK_SIZE, CHUNK_SIZE, 0},

	_normals {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1},
	_coord {0, 0,  0, TEX_FACTOR,  TEX_FACTOR, 0,  TEX_FACTOR, TEX_FACTOR},
	_indices {0, 1, 2, 3},
	_tex(tex) {

	_corners[0] = sf::Vector3f(_chunkPos.x * CHUNK_SIZE, _chunkPos.y * CHUNK_SIZE,0);
  _corners[1] = sf::Vector3f(_chunkPos.x * CHUNK_SIZE, (_chunkPos.y+1) * CHUNK_SIZE,0);
  _corners[2] = sf::Vector3f((_chunkPos.x+1) * CHUNK_SIZE, (_chunkPos.y+1) * CHUNK_SIZE,0);
  _corners[3] = sf::Vector3f((_chunkPos.x+1) * CHUNK_SIZE, _chunkPos.y * CHUNK_SIZE,0);

    // vbo

  glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glBufferData(	GL_ARRAY_BUFFER, (32*sizeof *_vertices), NULL, GL_STATIC_DRAW);

  glBufferSubData(GL_ARRAY_BUFFER, 0, (12*sizeof *_vertices), _vertices);
	glBufferSubData(GL_ARRAY_BUFFER, (12*sizeof *_vertices), (8*sizeof *_coord), _coord);
  glBufferSubData(GL_ARRAY_BUFFER, (12*sizeof *_vertices) + (8*sizeof *_coord), (12*sizeof *_normals), _normals);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // ibo

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof *_indices, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, 4 * sizeof *_indices, _indices);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Ocean::draw() const {
  glBindTexture(GL_TEXTURE_2D, _tex);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(12*sizeof *_vertices));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(12*sizeof *_vertices + 8*sizeof *_coord));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

  glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glBindTexture(GL_TEXTURE_2D, 0);
}
