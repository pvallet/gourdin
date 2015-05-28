#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "igMovingElement.h"

class Controllable : public igMovingElement {
public:
	Controllable(sf::Vector2f position, std::string filename);

	inline virtual abstractType getAbstractType() const {return CTRL;}

	virtual void update(sf::Time elapsed, float theta);
	virtual void setTarget(sf::Vector2f t);

	sf::Vector2f target;

};
