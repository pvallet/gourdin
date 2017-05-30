#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "animationManager.h"
#include "igElement.h"
#include "utils.h"

class igMovingElement : public igElement {
public:
	igMovingElement(sf::Vector2f position, AnimationManager graphics);
	virtual ~igMovingElement() {}

	void launchAnimation (ANM_TYPE type);
	virtual void updateDisplay(sf::Time elapsed, float theta); // Update sprite
	virtual void update(sf::Time elapsed); // Update pos and inner statuses
	virtual void die();

	inline size_t getTexID() const {return _graphics.getTexID();}

	inline virtual float getMaxHeightFactor() const {return _graphics.getMaxHeightFactor();}
	inline sf::Vector2f getDirection() const {return _direction;}
	inline float getSpeed() const {return _speed;}
	inline bool isDead() const {return _dead;}

protected:
	void setDirection(sf::Vector2f direction);

	float _speed; // Distance per second
	bool _dead;

	AnimationManager _graphics;

private:
	// Normalized vector towards the target
	// It is private to guarantee a correct normalization
	sf::Vector2f _direction;
};
