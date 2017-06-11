#include "igElement.h"

#include <cmath>
#include <iostream>
#include <ctime>

#include "camera.h"
#include "utils.h"
#include "vecUtils.h"

igElement::igElement(glm::vec2 position) :
	_pos(position),
	_camOrientation(0.f) {

	_orientation = RANDOMF * 360.f;
}

void igElement::updateDisplay(sf::Time elapsed, float theta) {
	setOrientation(_orientation + _camOrientation - theta); // Orientation moves opposite to the camera

	_camOrientation = theta;

	(void) elapsed;
}

void igElement::setVertices() {
	_vertices[0] = 0; _vertices[1]  =  _size.x/2 + _offset.x; _vertices[2]  = _size.y + _offset.y;
	_vertices[3] = 0; _vertices[4]  = -_size.x/2 + _offset.x; _vertices[5]  = _size.y + _offset.y;
	_vertices[6] = 0; _vertices[7]  = -_size.x/2 + _offset.x; _vertices[8]  =       0 + _offset.y;
	_vertices[9] = 0; _vertices[10] =  _size.x/2 + _offset.x; _vertices[11] =       0 + _offset.y;
}

void igElement::setPosArray() {
	for (size_t i = 0; i < 4; i++) {
		_posArray[3*i]     = _pos.x;
		_posArray[3*i + 1] = _pos.y;
		_posArray[3*i + 2] = _height;
	}
}

void igElement::setTexCoord(glm::vec4 rect) {
	_coord2D[0] = rect.x + rect.z;
	_coord2D[1] = rect.y;
	_coord2D[2] = rect.x;
	_coord2D[3] = rect.y;
	_coord2D[4] = rect.x;
	_coord2D[5] = rect.y + rect.w;
	_coord2D[6] = rect.x + rect.z;
	_coord2D[7] = rect.y + rect.w;
}

void igElement::setLayer(size_t layer) {
	for (size_t i = 0; i < 4; i++) {
		_layer[i] = layer;
	}
}

void igElement::setOrientation(float nOrientation) {
	_orientation = nOrientation;

	if (_orientation < 0.f)
		_orientation += 360.f + 360 * (int) (-_orientation / 360);
	else
		_orientation -= 360.f * (int) (_orientation / 360);
}
