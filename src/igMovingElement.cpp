#include "igMovingElement.h"
#include <cmath>
#include <iostream>

igMovingElement::igMovingElement(sf::Vector2f position, std::string filename) :
	speed(10.), 
	moving(false), 
	igElement(position, filename) {

}

void igMovingElement::stop() {
	graphics.stop();
	moving = false;
}

void igMovingElement::update(sf::Time elapsed, float theta) {
	igElement::update(elapsed, theta);

	if (direction.x == 0. && direction.y == 0.) {
		stop();
	}

	else {
		pos.x += direction.x * speed * elapsed.asSeconds();
		pos.y += direction.y * speed * elapsed.asSeconds();
	}
}


