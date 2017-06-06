#include "lion.h"

#include <iostream>

#include "antilope.h"
#include "vecUtils.h"

Lion::Lion(sf::Vector2f position, AnimationManager graphics, const TerrainGeometry& terrainGeometry) :
	Controllable(position, graphics, terrainGeometry),
	_stamina(100),
	_catchBreathSpeed(25.f),
	_loseBreathSpeed(10.f),
	_speedWalking(10.f),
	_speedRunning(21.f),
	_range(7.f),
	_status(WAITING) {

	_speed = _speedWalking;
	_animAttack = graphics.getAnimationTime(ATTACK);
}

void Lion::update(sf::Time elapsed) {
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
			setDirection(_prey->getPos() - _pos);
			_speed = _prey->getSpeed() * 0.9f;

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

	Controllable::update(elapsed);
}

void Lion::stop() {
	_speed = _speedWalking;
	_status = WAITING;
	Controllable::stop();
}

void Lion::beginRunning() {
	if (_target != _pos && _stamina > 0.f) {
		_status = RUNNING;
		_speed = _speedRunning;
		launchAnimation(RUN);
	}
}

void Lion::beginWalking() {
	if (_target != _pos) {
		_status = WALKING;
		_speed = _speedWalking;
		launchAnimation(WALK);
	}
}

void Lion::beginAttacking() {
	if (_status != ATTACKING) {
		_status = ATTACKING;
		_speed = 0.f;
		launchAnimation(ATTACK);
		_beginAttack.restart();
	}
}

void Lion::setTarget(sf::Vector2f t, ANM_TYPE anim) {
	if (_status == RUNNING)
		Controllable::setTarget(t, RUN);
	else {
		Controllable::setTarget(t, WALK);

		if (_status == WAITING) {
			beginWalking();
		}
	}
}

void Lion::kill(const std::list<igMovingElement*>& neighbors) {
	float distance;
	igMovingElement* closest = nullptr;
	float nearestDist = _range;

	for (auto it = neighbors.begin(); it != neighbors.end(); it++) {
		if (*it != this) {
			Antilope *atlp = dynamic_cast<Antilope*>(*it);
			if (atlp) {
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
