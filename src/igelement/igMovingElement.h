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

	virtual bool needsToUpdateHeight() const {return true;}

	virtual Animals getAnimalType() const = 0;

	void launchAnimation (ANM_TYPE type);
	virtual void updateDisplay(sf::Time elapsed, float theta);
	virtual void update(sf::Time elapsed);
	virtual void stop();
	virtual void die();

	inline size_t getTexID() const {return _graphics.getTexID();}

	inline virtual float getMaxHeightFactor() const {return _graphics.getMaxHeightFactor();}
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
