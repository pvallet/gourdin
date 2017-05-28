#include "ocean.h"
#include "vecUtils.h"

Ocean::Ocean(float oversizeFactor) :
	_vertices{
       -MAX_COORD*oversizeFactor,    -MAX_COORD*oversizeFactor,    0,
			 MAX_COORD*(1+oversizeFactor), -MAX_COORD*oversizeFactor,    0,
			 -MAX_COORD*oversizeFactor,    MAX_COORD*(1+oversizeFactor), 0,
			 MAX_COORD*(1+oversizeFactor), MAX_COORD*(1+oversizeFactor), 0},

	_normals {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1},
	_coord {0, 0,
          TEX_FACTOR*NB_CHUNKS*(1+2*oversizeFactor), 0,
          0, TEX_FACTOR*NB_CHUNKS*(1+2*oversizeFactor),
          TEX_FACTOR*NB_CHUNKS*(1+2*oversizeFactor), TEX_FACTOR*NB_CHUNKS*(1+2*oversizeFactor)},
	_indices {0, 1, 2, 3} {

  // vbo
	glGenBuffers(1, &_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glBufferData(	GL_ARRAY_BUFFER, sizeof(_vertices) + sizeof(_coord) + sizeof(_normals), NULL, GL_STATIC_DRAW);

  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(_vertices), &_vertices[0]);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(_vertices) , sizeof(_normals), &_normals[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(_vertices) + sizeof(_normals), sizeof(_coord), &_coord[0]);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // ibo

  glGenBuffers(1, &_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices), NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(_indices), &_indices[0]);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Ocean::~Ocean() {
	glDeleteBuffers(1, &_vbo);
  glDeleteBuffers(1, &_ibo);
}

void Ocean::draw() const {
	glBindTexture(GL_TEXTURE_2D, _tex);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(_vertices)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(_vertices) + sizeof(_normals)));

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
