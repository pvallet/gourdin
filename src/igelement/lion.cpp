#include "lion.h"

#include "antilope.h"

size_t Lion::_nbKilled = 0;

Lion::Lion(glm::vec2 position, AnimationManager graphics, const TerrainGeometry& terrainGeometry) :
	Controllable(position, graphics, terrainGeometry),
	_stamina(100),
	_catchBreathSpeed(25.f),
	_loseBreathSpeed(10.f),
	_speedWalking(10.f),
	_speedRunning(18.f),
	_rangeAttack(8.f),
	_rangeChase(13.f),
	_status(LionStatus::WAITING),
	_msAnimAttack(2.0f * graphics.getAnimationTime(ANM_TYPE::ATTACK)-150) {

	_speed = _speedWalking;
}

void Lion::update(int msElapsed) {
	if (_status == LionStatus::RUNNING || _status == LionStatus::CHASING) {
		_stamina -= msElapsed * _loseBreathSpeed / 1000.f;
		if (_stamina <= 0.f) {
			_stamina = 0.f;
			beginWalking();
		}

		if (_status == LionStatus::CHASING) {
			_target = _prey->getPos();
			setDirection(_prey->getPos() - _pos);
		}
	}

	else if (_status == LionStatus::ATTACKING) {
		_target = _prey->getPos();
		setDirection(_prey->getPos() - _pos);

		_speed = _prey->getSpeed() * 0.8f;

		if (_beginAttack.getElapsedTime() >= _msAnimAttack) {
			_prey->die();
			stop();
			_nbKilled++;
		}
	}

	else {
		_stamina += msElapsed * _catchBreathSpeed / 1000.f;
		if (_stamina >= 100.f)
			_stamina = 100.f;
	}

	Controllable::update(msElapsed);
}

void Lion::stop() {
	_speed = _speedWalking;
	_status = LionStatus::WAITING;
	Controllable::stop();
}

void Lion::beginRunning() {
	if (_target != _pos && _stamina > 0.f) {
		_status = LionStatus::RUNNING;
		_speed = _speedRunning;
		launchAnimation(ANM_TYPE::RUN);
	}
}

void Lion::beginWalking() {
	if (_target != _pos) {
		_status = LionStatus::WALKING;
		_speed = _speedWalking;
		launchAnimation(ANM_TYPE::WALK);
	}
}

void Lion::beginAttacking() {
	if (_status != LionStatus::ATTACKING) {
		_status = LionStatus::ATTACKING;
		_speed = 0.f;
		launchAnimation(ANM_TYPE::ATTACK);
		_beginAttack.restart();
	}
}

void Lion::beginChasing() {
	if (_status != LionStatus::CHASING && _status != LionStatus::ATTACKING && _status != LionStatus::WALKING) {
		_status = LionStatus::CHASING;
		_speed = _speedRunning;
		launchAnimation(ANM_TYPE::RUN);
	}
}

void Lion::setTarget(glm::vec2 t, ANM_TYPE anim) {
	(void) anim;
	if (_status == LionStatus::RUNNING || _status == LionStatus::CHASING)
		Controllable::setTarget(t, ANM_TYPE::RUN);
	else if (_status == LionStatus::WAITING) {
		Controllable::setTarget(t, ANM_TYPE::WALK);
		beginWalking();
	}
	else if (_status == LionStatus::WALKING)
		Controllable::setTarget(t,ANM_TYPE::WALK);
}

void Lion::updateState(const std::list<igMovingElement*>& neighbors) {
	float distance;
	igMovingElement* closest = nullptr;
	float nearestDist = _rangeChase;

	for (auto it = neighbors.begin(); it != neighbors.end(); it++) {
		if (*it != this) {
			Antilope *atlp = dynamic_cast<Antilope*>(*it);
			if (atlp) {
				distance = glm::length(_pos - atlp->getPos());

				if (distance < _rangeChase) {
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
		if (nearestDist < _rangeAttack)
			beginAttacking();
		else
			beginChasing();
	}
}
