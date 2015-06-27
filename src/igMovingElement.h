#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "igElement.h"

enum MovingType {PREY, HUNTER};

class igMovingElement : public igElement {
public:
	igMovingElement(sf::Vector2<double> position, AnimationManager _graphics);
	inline virtual abstractType getAbstractType() const {return igME;}
	virtual MovingType getMovingType() const = 0;

	void draw() const;

	void launchAnimation (ANM_TYPE type);
	virtual void update(sf::Time elapsed, float theta);
	virtual void stop();
	virtual void die();

	inline virtual float getMaxHeightFactor() {return graphics.getMaxHeightFactor();}
	inline sf::Vector2<double> getDirection() const {return direction;}
	inline float getSpeed() const {return speed;}
	inline bool isDead() const {return dead;}

protected:
	float speed; // Distance per second
	bool moving;
	bool dead;

	AnimationManager graphics;

	sf::Vector2<double> direction; // Normalized vector towards the target
};
