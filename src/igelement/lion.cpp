#include "lion.h"

#include <iostream>

#include "antilope.h"
#include "vecUtils.h"

Lion::Lion(sf::Vector2f position, AnimationManager graphics) :
	Controllable(position, graphics),
	_stamina(100),
	_catchBreathSpeed(25.),
	_loseBreathSpeed(10.),
	_speedWalking(7.),
	_speedRunning(15.),
	_range(5.),
	_status(WAITING) {

	_size *= 5.f;
	_speed = _speedWalking;
	_animAttack = graphics.getAnimationTime(ATTACK);
}

void Lion::update(sf::Time elapsed, float theta) {
	switch(_status) {
		case RUNNING:
			_stamina -= elapsed.asSeconds() * _loseBreathSpeed;
			if (_stamina <= 0.f) {
				_stamina = 0.f;
				beginWalking();
			}
			break;

		case ATTACKING:
			_target = _prey->getPos();
			_direction = _prey->getPos() - _pos;
			_direction /= vu::norm(_direction);
			_speed = _prey->getSpeed() - 1.f;

			if (_beginAttack.getElapsedTime() >= _animAttack) {
				_prey->die();
				stop();
			}
			break;

		default:
			_stamina += elapsed.asSeconds() * _catchBreathSpeed;
			if (_stamina >= 100.f)
				_stamina = 100.f;
			break;
	}

	Controllable::update(elapsed, theta);
}

void Lion::stop() {
	igMovingElement::stop();
	_status = WAITING;
}

void Lion::beginRunning() {
	if (_stamina > 0.) {
		_moving = true;
		_status = RUNNING;
		_speed = _speedRunning;
		launchAnimation(RUN);
	}
}

void Lion::beginWalking() {
	_moving = true;
	_status = WALKING;
	_speed = _speedWalking;
	launchAnimation(WALK);
}

void Lion::beginAttacking() {
	if (_status != ATTACKING) {
		_status = ATTACKING;
		_speed = 0.f;
		launchAnimation(ATTACK);
		_beginAttack.restart();
	}
}

void Lion::setTarget(sf::Vector2f t) {
	Controllable::setTarget(t);
	if (_status == WAITING) {
		beginWalking();
	}
}

void Lion::kill(std::vector<igMovingElement*> neighbors) {
	float distance;
	igMovingElement* closest = NULL;
	float nearestDist = _range;

	for (unsigned int i = 0 ; i < neighbors.size() ; i++) {
		if (neighbors[i] != this) {
			if (dynamic_cast<Antilope*>(neighbors[i])) {
				distance = vu::norm(_pos - neighbors[i]->getPos());

				if (distance < _range) {
					if (distance < nearestDist) {
						nearestDist = distance;
						closest = neighbors[i];
					}
				}
			}
		}
	}

	if (nearestDist != _range && !closest->isDead()) {
		beginAttacking();
		_prey = closest;
	}
}
