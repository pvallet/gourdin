#include "igElement.h"

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <cmath>
#include <iostream>
#include <ctime>

#include "utils.h"

#define BUFFER_OFFSET(a) ((char*)NULL + (a))

igElement::igElement(sf::Vector2f position) :
	_pos(position),
	_camOrientation(0.),
	_visible(false) {

	_orientation = random() * 360.f;

	_vertices = new float[12];
  _coord2D = new float[8];
  _indices = new GLuint[4];

  for (size_t i = 0 ; i < 12 ; i++)
    _vertices[i] = 0.f;

	// Default coordinates that pastes the whole texture to the rectangle
	_coord2D[0] = 1;
	_coord2D[1] = 0;
  _coord2D[2] = 0;
  _coord2D[3] = 0;
  _coord2D[4] = 0;
  _coord2D[5] = 1;
	_coord2D[6] = 1;
	_coord2D[7] = 1;

  glGenBuffers(1, &_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glBufferData(GL_ARRAY_BUFFER, (12*sizeof *_vertices) + (8*sizeof *_coord2D), NULL, GL_STREAM_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  for (size_t i = 0 ; i < 4 ; i++) {
    _indices[i] = i;
  }

  glGenBuffers(1, &_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof *_indices, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, 4 * sizeof *_indices, _indices);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	_glCheckError();
}

igElement::~igElement() {
	glDeleteBuffers(1, &_vbo);
	glDeleteBuffers(1, &_ibo);
}

void igElement::update(sf::Time elapsed, float theta) {
	setOrientation(_orientation + _camOrientation - theta); // Orientation moves opposite to the camera

	_camOrientation = theta;
}

void igElement::set3DCorners(glm::vec3 nCorners[4]) {
	for (size_t i = 0 ; i < 4 ; i++) {
		_vertices[3*i]   = nCorners[i].x;
		_vertices[3*i+1] = nCorners[i].y;
		_vertices[3*i+2] = nCorners[i].z;
	}

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glBufferSubData(GL_ARRAY_BUFFER, 0, (12*sizeof *_vertices), _vertices);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

	_glCheckError();
}

void igElement::setOrientation(float nOrientation) {
	_orientation = nOrientation;

	if (_orientation < 0.)
		_orientation += 360. + 360 * (int) (-_orientation / 360);
	else
		_orientation -= 360. * (int) (_orientation / 360);
}

void igElement::draw() const {
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(12*sizeof *_vertices));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
