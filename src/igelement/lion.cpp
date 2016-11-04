#include "lion.h"

#include <iostream>

#include "antilope.h"
#include "vecUtils.h"

Lion::Lion(sf::Vector2f position, AnimationManager graphics) :
	Controllable(position, graphics),
	_stamina(100),
	_catchBreathSpeed(25.f),
	_loseBreathSpeed(10.f),
	_speedWalking(10.f),
	_speedRunning(21.f),
	_range(5.f),
	_status(WAITING) {

	_size *= 7.f;
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
	if (_stamina > 0.f) {
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

void Lion::kill(const std::set<igMovingElement*>& neighbors) {
	float distance;
	igMovingElement* closest = NULL;
	float nearestDist = _range;

	for (auto it = neighbors.begin(); it != neighbors.end(); it++) {
		if (*it != this) {
			Antilope *atlp;
			if (atlp = dynamic_cast<Antilope*>(*it)) {
				distance = vu::norm(_pos - atlp->getPos());

				if (distance < _range) {
					if (distance < nearestDist) {
						nearestDist = distance;
						closest = atlp;
					}
				}
			}
		}
	}

	if (closest) {
		_prey = closest;
		beginAttacking();
	}
}
