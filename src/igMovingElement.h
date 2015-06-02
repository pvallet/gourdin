#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "igElement.h"

enum MovingType {PREY, HUNTER};

class igMovingElement : public igElement {
public:
	igMovingElement(sf::Vector2f position, AnimationManager _graphics);
	inline virtual abstractType getAbstractType() const {return igME;}
	virtual MovingType getMovingType() const = 0;

	virtual void update(sf::Time elapsed, float theta);
	virtual void stop();
	virtual void die();

	inline sf::Vector2f getDirection() const {return direction;}
	inline float getSpeed() const {return speed;}
	inline bool isDead() const {return dead;}

protected:
	float speed; // Distance per second
	bool moving;
	bool dead;

	sf::Vector2f direction; // Normalized vector towards the target
};
