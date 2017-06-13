#include "igMovingElement.h"
#include "vecUtils.h"
#include "utils.h"

#include <cmath>
#include <iostream>

igMovingElement::igMovingElement(glm::vec2 position, AnimationManager graphics, const TerrainGeometry& terrainGeometry) :
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
		glm::vec2 oldSize = _size;

		_size.x /= _graphics.getRawSize().x;
		_size.y /= _graphics.getRawSize().y;
		setLayer(_graphics.launchAnimation(type));
		_size.x *= _graphics.getRawSize().x;
		_size.y *= _graphics.getRawSize().y;

		_offset.y += (oldSize.y - _size.y) / 2.f;
		setVertices();
	}
}

void igMovingElement::updateDisplay(int msElapsed, float theta) {
	igElement::updateDisplay(msElapsed, theta);
	_graphics.update(msElapsed, getOrientation());

	if (!_dead) {
		if (_direction.x != 0.f || _direction.y != 0.f) {
			float ori = vu::angle(glm::vec2(1.0f,0.0f), _direction);
			setOrientation(ori - _camOrientation);
		}
	}

	setTexCoord(_graphics.getCurrentSpriteRect());
}

void igMovingElement::update(int msElapsed) {
	glm::vec2 newPos = _pos + _direction * _speed * (msElapsed / 1000.f);

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
    setDirection(glm::vec2(0,0));
    launchAnimation(WAIT);
  }
}

void igMovingElement::setDirection(glm::vec2 direction) {
	_direction = direction;
	float length = glm::length(_direction);

	if (length != 0)
		_direction /= length;
}
