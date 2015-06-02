#include "controllable.h"
#include "vecUtils.h"
#include <iostream>

Controllable::Controllable(sf::Vector2f position, AnimationManager _graphics) :
 	igMovingElement(position, _graphics) {
	
}

void Controllable::update(sf::Time elapsed, float theta) {
	igMovingElement::update(elapsed, theta);
	
	if (!dead) {
		// The element has gone too far
		if (vu::dot((target - pos), direction) < 0) {
			pos = target;
			stop();
		}
	}
}

void Controllable::setTarget(sf::Vector2f t) {
	if (!dead) {
		moving = true;
		target = t;

		direction = (t-pos);
		direction /= vu::norm(direction);
	}
}

