#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "igMovingElement.h"

class Controllable : public igMovingElement {
public:
	Controllable(sf::Vector2f position, AnimationManager _graphics);

	virtual void update(sf::Time elapsed, float theta);
	virtual void setTarget(sf::Vector2f t);

protected:
	sf::Vector2f _target;

};
