#include "igMovingElement.h"
#include "vecUtils.h"
#include "utils.h"

#include <cmath>
#include <iostream>

igMovingElement::igMovingElement(sf::Vector2f position, AnimationManager graphics, const TerrainGeometry& terrainGeometry) :
	igElement(position),
	_dead(false),
	_graphics(graphics),
	_terrainGeometry(terrainGeometry) {
	_size = _graphics.getRawSize();
	_size /= _size.y;
	_size *= _graphics.getParameters().size;
	_speed = _graphics.getParameters().speed;

	launchAnimation(WAIT);
}

void igMovingElement::launchAnimation(ANM_TYPE type) {
	if (!_dead) {
		sf::Vector2f oldSize = _size;

		_size.x /= _graphics.getRawSize().x;
		_size.y /= _graphics.getRawSize().y;
		setLayer(_graphics.launchAnimation(type));
		_size.x *= _graphics.getRawSize().x;
		_size.y *= _graphics.getRawSize().y;

		_offset.y += (oldSize.y - _size.y) / 2.f;
		setVertices();
	}
}

void igMovingElement::updateDisplay(sf::Time elapsed, float theta) {
	igElement::updateDisplay(elapsed, theta);
	_graphics.update(elapsed, getOrientation());

	if (!_dead) {
		if (_direction.x != 0.f || _direction.y != 0.f) {
			float ori = vu::angle(sf::Vector2f(1.0f,0.0f), _direction);
			setOrientation(ori - _camOrientation);
		}
	}

	setTexCoord(_graphics.getCurrentSprite());
}

void igMovingElement::update(sf::Time elapsed) {
	sf::Vector2f newPos = _pos + _direction * _speed * elapsed.asSeconds();

	if (_terrainGeometry.isWater(newPos, 0))
		stop();
	else
		_pos = newPos;
}

void igMovingElement::die() {
	launchAnimation(DIE);
	_speed = 0.f;
	_dead = true;
}

void igMovingElement::stop() {
	if (!_dead) {
    setDirection(sf::Vector2f(0,0));
    launchAnimation(WAIT);
  }
}

void igMovingElement::setDirection(sf::Vector2f direction) {
	_direction = direction;
	float norm = vu::norm(_direction);

	if (norm != 0)
		_direction /= norm;
}
