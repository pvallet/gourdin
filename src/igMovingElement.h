#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "igElement.h"

enum MovingType {BOID, HUNTER};

class igMovingElement : public igElement {
public:
	igMovingElement(sf::Vector2f position, std::string filename);
	inline virtual abstractType getAbstractType() const {return igME;}
	virtual MovingType getMovingType() const = 0;

	virtual void update(sf::Time elapsed, float theta);
	virtual void stop();

protected:
	const float speed; // Distance per second
	bool moving;

	sf::Vector2f direction; // Normalized vector towards the target
};
