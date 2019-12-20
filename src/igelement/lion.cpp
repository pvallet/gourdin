#include "lion.h"

#include "antilope.h"

#define BASE_HUNGER_RATE 0.03f

Lion::Lion(glm::vec2 position, AnimationManager graphics, const TerrainGeometry& terrainGeometry) :
	Super(position, graphics, terrainGeometry),
	_hunger(100.f),
	_speedWalking(10.f),
	_speedRunning(18.f),
	_rangeAttack(8.f),
	_status(RESTING),
	_msAnimAttack(2.0f * graphics.getAnimationTime(ATTACK)-150) {

	_speed = _speedWalking;

	_hungerRates[RESTING]   = BASE_HUNGER_RATE * 0.8f;
	_hungerRates[ROAMING]   = BASE_HUNGER_RATE * 1.0f;
	_hungerRates[ATTACKING] = BASE_HUNGER_RATE * 1.0f;
	_hungerRates[CHASING]   = BASE_HUNGER_RATE * 1.0f;
}

void Lion::update(int msElapsed) {
	_hunger -= _hungerRates[_status];

	if (_hunger < 0.f) {
		_hunger = 0.f;
		die();
	}

	if (_hunger < 80.f && _status == RESTING) {
		glm::vec2 toTarget;
		float r = 50.f + sqrt(RANDOMF) * 50.f;
		float theta = RANDOMF * 2*M_PI;

		toTarget.x = r*cos(theta);
		toTarget.y = r*sin(theta);

		setTarget(getPos() + toTarget, WALK);
		beginRoaming();
	}

	if (_prey != nullptr && _status != RESTING) {
		_target = _prey->getPos();
		setDirection(_prey->getPos() - _pos);
	}

	if (_status == ATTACKING) {
			_speed = _prey->getSpeed() * 0.8f;

		if (_beginAttack.getElapsedTime() >= _msAnimAttack) {
			_prey->die();
			_hunger = std::min(100.f, _hunger + 60.f);
			stop();
		}
	}

	Super::update(msElapsed);
}

void Lion::stop() {
	_speed = _speedWalking;
	_status = RESTING;
	Super::stop();
}

void Lion::beginRoaming() {
	if (_target != _pos) {
		_status = ROAMING;
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

void Lion::beginChasing() {
	if (_status != CHASING && _status != ATTACKING) {
		_status = CHASING;
		_speed = _speedRunning;
		launchAnimation(RUN);
	}
}

void Lion::updateState(const std::list<igMovingElement*>& neighbors) {
	float distance;
	igMovingElement* closest = nullptr;
	float nearestDist = 50.f;

	for (auto it = neighbors.begin(); it != neighbors.end(); it++) {
		if (*it != this) {
			Antilope *atlp = dynamic_cast<Antilope*>(*it);
			if (atlp) {
				distance = glm::length(_pos - atlp->getPos());

				if (distance < nearestDist) {
					nearestDist = distance;
					closest = atlp;
				}
			}
		}
	}

	_prey = closest;

	if (_prey && (_status == ROAMING || _status == CHASING)) {
		if (nearestDist < _rangeAttack)
			beginAttacking();
		else if (nearestDist < 30.f)
			beginChasing();
	}
}
