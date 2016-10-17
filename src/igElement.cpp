#include "igElement.h"
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <cmath>
#include <iostream>
#include <ctime>

#include "utils.h"

#define BUFFER_OFFSET(a) ((char*)NULL + (a))

igElement::igElement(sf::Vector2<double> position) :
	_pos(position),
	_height(3.),
	_camOrientation(0.),
	_visible(false) {

	_orientation = randomF() * 360.f;

	_vertices = new float[12];
  _coord2D = new int[8];
  _indices = new GLuint[4];

  for (int i = 0 ; i < 12 ; i++)
    _vertices[i] = 0.f;

  for (int i = 0 ; i < 8 ; i++)
    _coord2D[i] = 0;

  glGenBuffers(1, &_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glBufferData(   GL_ARRAY_BUFFER, (12*sizeof *_vertices) + (8*sizeof *_coord2D), NULL, GL_STREAM_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  for (int i = 0 ; i < 4 ; i++) {
    _indices[i] = i;
  }

  glGenBuffers(1, &_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof *_indices, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, 4 * sizeof *_indices, _indices);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

igElement::~igElement() {
	glDeleteBuffers(1, &_vbo);
	glDeleteBuffers(1, &_ibo);
	// delete[] _vertices;
	// delete[] _coord2D;
	// delete[] _indices;
}

void igElement::update(sf::Time elapsed, float theta) {
	setOrientation(_orientation + _camOrientation - theta); // Orientation moves opposite to the camera

	_camOrientation = theta;
}

void igElement::set3DCorners(glm::vec3 nCorners[4]) {
	for (int i = 0 ; i < 4 ; i++) {
		_vertices[3*i]   = nCorners[i].x;
		_vertices[3*i+1] = nCorners[i].y;
		_vertices[3*i+2] = nCorners[i].z;
	}

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glBufferSubData(GL_ARRAY_BUFFER, 0, (12*sizeof *_vertices), _vertices);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void igElement::setOrientation(float nOrientation) {
	_orientation = nOrientation;

	if (_orientation < 0.)
		_orientation += 360. + 360 * (int) (-_orientation / 360);
	else
		_orientation -= 360. * (int) (_orientation / 360);
}
