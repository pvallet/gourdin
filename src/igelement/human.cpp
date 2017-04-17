#include "human.h"

Human::Human(sf::Vector2f position, AnimationManager graphics) :
	Controllable(position, graphics) {

	_size *= 6.f;
	_speed = 5.f;

	launchAnimation(WAIT);
}
