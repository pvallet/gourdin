#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "igElement.h"

class igMovingElement : public igElement {
public:
	igMovingElement(sf::Vector2f position, std::string filename);
	inline virtual GraphicType getType() {return igME;}

	virtual void update(sf::Time elapsed, float theta);

	void setTarget(sf::Vector2f t);
protected:
	const float speed; // Distance per second
	bool moving;
	sf::Vector2f target;
	sf::Vector2f direction; // Normalized vector towards the target
};
