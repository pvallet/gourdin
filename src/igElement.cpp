#include "igElement.h"
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <cmath>
#include <iostream>

igElement::igElement(sf::Vector2f position, std::string filename) :
	pos(position),
	height(3.),
	graphics(filename),
	orientation(90.),
	camOrientation(0.) {

	corners3 = new sf::Vector3f[4];
	
	for (int i = 0 ; i < 4 ; i++) {
		corners3[i].x = 0.;
		corners3[i].y = 0.;
		corners3[i].z = 0.;
	}
}

igElement::~igElement() {
	delete[] corners3;
}

void igElement::update(sf::Time elapsed, float theta) {
	graphics.update(elapsed, orientation);
	
	setOrientation(orientation + camOrientation - theta); // Orientation moves opposite to the camera

	camOrientation = theta;
}

void igElement::set3DCorners(sf::Vector3f nCorners[4]) {
	for (int i = 0 ; i < 4 ; i++) {
		corners3[i] = nCorners[i];
	}
}

void igElement::setOrientation(float nOrientation) {
	orientation = nOrientation;

	if (orientation < 0.)
		orientation += 360. + 360 * (int) (-orientation / 360);
	else
		orientation -= 360. * (int) (orientation / 360);
}
