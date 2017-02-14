#include "igMovingElement.h"
#include "vecUtils.h"
#include "utils.h"

#include <cmath>
#include <iostream>

igMovingElement::igMovingElement(sf::Vector2f position, AnimationManager graphics) :
	igElement(position),
	_speed(0.f),
	_moving(false),
	_dead(false),
	_graphics(graphics) {
	_size = _graphics.getRawSize();
	_size /= _size.y;
	launchAnimation(WAIT);
}

void igMovingElement::launchAnimation(ANM_TYPE type) {
	_size.x /= _graphics.getRawSize().x;
	_size.y /= _graphics.getRawSize().y;
	setLayer(_graphics.launchAnimation(type));
	_size.x *= _graphics.getRawSize().x;
	_size.y *= _graphics.getRawSize().y;
}

void igMovingElement::stop() {
	launchAnimation(WAIT);
	_moving = false;
	_speed = 0.f;
}

void igMovingElement::update(sf::Time elapsed, float theta) {
	_graphics.update(elapsed, getOrientation());
	igElement::update(elapsed, theta);

	if (!_dead) {
		if (_direction.x != 0.f || _direction.y != 0.f) {
			float ori = vu::angle(sf::Vector2f(1.0f,0.0f), _direction);
			setOrientation(ori - _camOrientation);

			_pos.x += _direction.x * _speed * elapsed.asSeconds();
			_pos.y += _direction.y * _speed * elapsed.asSeconds();
		}
	}

	setTexCoord(_graphics.getCurrentSprite());
}

void igMovingElement::die() {
	launchAnimation(DIE);
	_speed = 0.f;
	_moving = false;
	_dead = true;
	_graphics.die();
}
