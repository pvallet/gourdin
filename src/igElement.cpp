#include "igElement.h"
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <cmath>
#include <iostream>
#include <ctime>

#include "utils.h"

#define BUFFER_OFFSET(a) ((char*)NULL + (a))

igElement::igElement(sf::Vector2<double> position) :
	pos(position),
	height(3.),
	camOrientation(0.),
	visible(false) {

	orientation = randomF() * 360.f;

	vertices = new float[12];
  coord2D = new int[8];
  indices = new GLuint[4];

  for (int i = 0 ; i < 12 ; i++)
    vertices[i] = 0.f;

  for (int i = 0 ; i < 8 ; i++)
    coord2D[i] = 0;

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glBufferData(   GL_ARRAY_BUFFER, (12*sizeof *vertices) + (8*sizeof *coord2D), NULL, GL_STREAM_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  for (int i = 0 ; i < 4 ; i++) {
    indices[i] = i;
  }

  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof *indices, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, 4 * sizeof *indices, indices);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

igElement::~igElement() {
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	delete[] vertices;
	delete[] coord2D;
	delete[] indices;
}

void igElement::update(sf::Time elapsed, float theta) {
	setOrientation(orientation + camOrientation - theta); // Orientation moves opposite to the camera

	camOrientation = theta;
}

void igElement::set3DCorners(sf::Vector3f nCorners[4]) {
	for (int i = 0 ; i < 4 ; i++) {
		vertices[3*i]   = nCorners[i].x;
		vertices[3*i+1] = nCorners[i].y;
		vertices[3*i+2] = nCorners[i].z;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glBufferSubData(GL_ARRAY_BUFFER, 0, (12*sizeof *vertices), vertices);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void igElement::setOrientation(float nOrientation) {
	orientation = nOrientation;

	if (orientation < 0.)
		orientation += 360. + 360 * (int) (-orientation / 360);
	else
		orientation -= 360. * (int) (orientation / 360);
}
