#include "skybox.h"

#include <iostream>

#define BUFFER_OFFSET(a) ((char*)NULL + (a))

Skybox::Skybox() :
	t(1000.),
	_vertices {
     -t,-t,-t,  -t, t,-t,  -t,-t, t,  -t, t, t, // XN
      t, t,-t,   t,-t,-t,   t, t, t,   t,-t, t, // XP
      t,-t,-t,  -t,-t,-t,   t,-t, t,  -t,-t, t, // YN
     -t, t,-t,   t, t,-t,  -t, t, t,   t, t, t, // YP
      t,-t,-t,  -t,-t,-t,   t, t,-t,  -t, t,-t, // ZN
     -t, t, t,   t, t, t,  -t,-t, t,   t,-t, t  // ZP
	},
  _indices {
     0,  1,  2,  2,  1,  3, // XN
     4,  5,  6,  6,  5,  7, // XP
     8,  9, 10, 10,  9, 11, // YN
    12, 13, 14, 14, 13, 15, // YP
    16, 17, 18, 18, 17, 19, // ZN
    20, 21, 22, 22, 21, 23  // ZP
  },
  _coord {
    0, 1,  1, 1,  0, 0,  1, 0,
		0, 1,  1, 1,  0, 0,  1, 0,
		0, 1,  1, 1,  0, 0,  1, 0,
		0, 1,  1, 1,  0, 0,  1, 0,
		0, 1,  1, 1,  0, 0,  1, 0,
		0, 1,  1, 1,  0, 0,  1, 0
  },
  _layer {
    0, 0, 0, 0,
    1, 1, 1, 1,
    2, 2, 2, 2,
    3, 3, 3, 3,
    4, 4, 4, 4,
    5, 5, 5, 5
  } {

  // vbo
	glGenBuffers(1, &_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glBufferData(	GL_ARRAY_BUFFER, sizeof(_vertices) + sizeof(_coord) + sizeof(_layer), NULL, GL_STATIC_DRAW);

  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(_vertices), &_vertices[0]);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(_vertices) , sizeof(_coord), &_coord[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(_vertices) + sizeof(_coord), sizeof(_layer), &_layer[0]);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // ibo

  glGenBuffers(1, &_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices), NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(_indices), &_indices[0]);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Skybox::~Skybox() {
	glDeleteBuffers(1, &_vbo);
  glDeleteBuffers(1, &_ibo);
}

void Skybox::load(std::string filename) {
  _texArray.loadTextures(6, filename);
}

void Skybox::draw() const {
	glBindTexture(GL_TEXTURE_2D_ARRAY, _texArray.texID);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(_vertices)));
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(_vertices) + sizeof(_coord)));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

  glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}