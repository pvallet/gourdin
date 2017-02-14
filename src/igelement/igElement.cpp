#include "igElement.h"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <ctime>

#include "camera.h"
#include "utils.h"

#define BUFFER_OFFSET(a) ((char*)NULL + (a))

igElement::igElement(sf::Vector2f position) :
	_pos(position),
	_camOrientation(0.f),
	_visible(false) {

	_orientation = RANDOMF * 360.f;

  for (size_t i = 0 ; i < 12 ; i++)
    _vertices[i] = 0.f;
}


void igElement::update(sf::Time elapsed, float theta) {
	setOrientation(_orientation + _camOrientation - theta); // Orientation moves opposite to the camera

	_camOrientation = theta;
}

void igElement::setTexCoord(sf::FloatRect rect) {
	_coord2D[0] = rect.left + rect.width;
	_coord2D[1] = rect.top;
	_coord2D[2] = rect.left;
	_coord2D[3] = rect.top;
	_coord2D[4] = rect.left;
	_coord2D[5] = rect.top  + rect.height;
	_coord2D[6] = rect.left + rect.width;
	_coord2D[7] = rect.top  + rect.height;
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

sf::IntRect igElement::getScreenCoord() const {
	Camera& cam = Camera::getInstance();

	sf::IntRect res;

	res.left = (_vertices[3] + 1.f) / 2.f * cam.getW();
	res.top = -(_vertices[1] + 1.f) / 2.f * cam.getH() + cam.getH();
	res.width =  (_vertices[0] - _vertices[3]) / 2.f * cam.getW();
	res.height = (_vertices[1] - _vertices[7]) / 2.f * cam.getH();

	return res;
}
