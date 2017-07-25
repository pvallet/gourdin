#include "ocean.h"
#include "utils.h"

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

  _vbo.bind();

  glBufferData(	GL_ARRAY_BUFFER, sizeof(_vertices) + sizeof(_coord) + sizeof(_normals), NULL, GL_STATIC_DRAW);

  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(_vertices), &_vertices[0]);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(_vertices) , sizeof(_normals), &_normals[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(_vertices) + sizeof(_normals), sizeof(_coord), &_coord[0]);

  VertexBufferObject::unbind();

  // ibo

	_ibo.bind();

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices), NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(_indices), &_indices[0]);

  IndexBufferObject::unbind();

	// vao

  _vao.bind();
	_vbo.bind();

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(_vertices)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(_vertices) + sizeof(_normals)));

	VertexBufferObject::unbind();

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	VertexArrayObject::unbind();
}

void Ocean::draw() const {
	_vao.bind();
	_texture->bind();
	_ibo.bind();

  glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

  IndexBufferObject::unbind();
  Texture::unbind();
	VertexArrayObject::unbind();
}
