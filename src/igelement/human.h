#pragma once

#include "controllable.h"

class Human : public Controllable {
public:
	Human(sf::Vector2f position, AnimationManager _graphics);
	virtual ~Human() {}
};
