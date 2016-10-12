#include "controllable.h"
#include "vecUtils.h"
#include <iostream>

Controllable::Controllable(sf::Vector2<double> position, AnimationManager graphics) :
 	igMovingElement(position, graphics) {

}

void Controllable::update(sf::Time elapsed, float theta) {
	igMovingElement::update(elapsed, theta);

	if (!_dead) {
		// The element has gone too far
		if (vu::dot((_target - _pos), _direction) < 0) {
			_pos = _target;
			stop();
		}
	}
}

void Controllable::setTarget(sf::Vector2<double> t) {
	if (!_dead) {
		_moving = true;
		_target = t;

		_direction = (t-_pos);
		_direction /= vu::norm(_direction);
	}
}