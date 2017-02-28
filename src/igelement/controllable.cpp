#include "controllable.h"
#include "vecUtils.h"
#include <iostream>

Controllable::Controllable(sf::Vector2f position, AnimationManager graphics) :
 	igMovingElement(position, graphics),
  _target(position) {

}

void Controllable::update(sf::Time elapsed) {
  if (!_dead) {
    igMovingElement::update(elapsed);
    // The element has gone too far
    if (vu::dot((_target - _pos), _direction) < 0) {
      _pos = _target;
      stop();
    }
  }
}

void Controllable::setTarget(sf::Vector2f t) {
	if (!_dead) {
		_moving = true;
		_target = t;

		_direction = (t-_pos);
		_direction /= vu::norm(_direction);
	}
}
