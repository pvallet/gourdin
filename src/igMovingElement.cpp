#include "igMovingElement.h"
#include "vecUtils.h"
#include <cmath>
#include <iostream>

igMovingElement::igMovingElement(sf::Vector2f position, AnimationManager _graphics) :
	speed(0.), 
	moving(false), 
	dead(false),
	igElement(position, _graphics) {

}

void igMovingElement::stop() {
	launchAnimation(WAIT);
	moving = false;
	speed = 0.f;
}

void igMovingElement::update(sf::Time elapsed, float theta) {
	igElement::update(elapsed, theta);

	if (!dead) {
		if (direction.x != 0. || direction.y != 0.) {
			float ori = vu::angle(sf::Vector2f(1.0f,0.0f), direction);
			setOrientation(ori - camOrientation);
			
			pos.x += direction.x * speed * elapsed.asSeconds();
			pos.y += direction.y * speed * elapsed.asSeconds();
		}
	}
}

void igMovingElement::die() {
	launchAnimation(DIE);
	speed = 0.;
	moving = false;
	dead = true;
	graphics.die();
}


