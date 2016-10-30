#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "igElement.h"
#include "animationManager.h"

class igMovingElement : public igElement {
public:
	igMovingElement(sf::Vector2f position, AnimationManager graphics);
	
	void draw() const;

	void launchAnimation (ANM_TYPE type);
	virtual void update(sf::Time elapsed, float theta);
	virtual void stop();
	virtual void die();

	inline virtual float getMaxHeightFactor() {return _graphics.getMaxHeightFactor();}
	inline sf::Vector2f getDirection() const {return _direction;}
	inline float getSpeed() const {return _speed;}
	inline bool isDead() const {return _dead;}

protected:
	float _speed; // Distance per second
	bool _moving;
	bool _dead;

	AnimationManager _graphics;

	sf::Vector2f _direction; // Normalized vector towards the target
};
