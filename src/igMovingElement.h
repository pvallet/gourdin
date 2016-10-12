#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "igElement.h"

enum MovingType {PREY, HUNTER};

class igMovingElement : public igElement {
public:
	igMovingElement(sf::Vector2<double> position, AnimationManager graphics);
	inline virtual abstractType getAbstractType() const {return igME;}
	virtual MovingType getMovingType() const = 0;

	void draw() const;

	void launchAnimation (ANM_TYPE type);
	virtual void update(sf::Time elapsed, float theta);
	virtual void stop();
	virtual void die();

	inline virtual float getMaxHeightFactor() {return _graphics.getMaxHeightFactor();}
	inline sf::Vector2<double> getDirection() const {return _direction;}
	inline float getSpeed() const {return _speed;}
	inline bool isDead() const {return _dead;}

protected:
	float _speed; // Distance per second
	bool _moving;
	bool _dead;

	AnimationManager _graphics;

	sf::Vector2<double> _direction; // Normalized vector towards the target
};
