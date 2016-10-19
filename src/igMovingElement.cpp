#include "igMovingElement.h"
#include "vecUtils.h"
#include "utils.h"
#include <cmath>
#include <iostream>

igMovingElement::igMovingElement(sf::Vector2<double> position, AnimationManager graphics) :
	igElement(position),
	_speed(0.),
	_moving(false),
	_dead(false),
	_graphics(graphics) {}

void igMovingElement::draw() const {
	const sf::Texture* texture = _graphics.getTexture();

  sf::Texture::bind(texture, sf::Texture::CoordinateType::Pixels);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribPointer(2, 2, GL_INT, GL_FALSE, 0, BUFFER_OFFSET(12*sizeof *_vertices));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(2);

  glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glBindTexture(GL_TEXTURE_2D, 0);
}

void igMovingElement::launchAnimation(ANM_TYPE type) {
	_height /= _graphics.getCurrentSprite().height;
	_graphics.launchAnimation(type);
	_height *= _graphics.getCurrentSprite().height;
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
		if (_direction.x != 0. || _direction.y != 0.) {
			float ori = vu::angle(sf::Vector2<double>(1.0f,0.0f), _direction);
			setOrientation(ori - _camOrientation);

			_pos.x += _direction.x * _speed * elapsed.asSeconds();
			_pos.y += _direction.y * _speed * elapsed.asSeconds();
		}
	}

	sf::IntRect rect = _graphics.getCurrentSprite();
	_coord2D[0] = rect.left;
	_coord2D[1] = rect.top;
	_coord2D[2] = rect.left + rect.width;
	_coord2D[3] = rect.top;
	_coord2D[4] = rect.left + rect.width;
	_coord2D[5] = rect.top + rect.height;
	_coord2D[6] = rect.left;
	_coord2D[7] = rect.top + rect.height;

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

	glBufferSubData(GL_ARRAY_BUFFER, (12*sizeof *_vertices), (8*sizeof *_coord2D), _coord2D);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void igMovingElement::die() {
	launchAnimation(DIE);
	_speed = 0.;
	_moving = false;
	_dead = true;
	_graphics.die();
}
