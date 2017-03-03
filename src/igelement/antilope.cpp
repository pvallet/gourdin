#include "antilope.h"

#include <cmath>
#include <iostream>

#include "lion.h"
#include "utils.h"
#include "vecUtils.h"

Antilope::Antilope(sf::Vector2f position, AnimationManager graphics) :
	igMovingElement(position, graphics),
	_lineOfSightStandard(50.f),
	_repulsionRadius(8.f),
	_orientationRadius(15.f),
	_attractionRadius(50.f),
	_speedWalking(7.f),
	_speedRunning(15.f),
	_aStatus(IDLE),
	_bStatus(ORIENTATION) {

	_size *= 7.f;
	_lineOfSight = _lineOfSightStandard;

	_averageRecovering = sf::seconds(3.f);
	_averageEating = sf::seconds(7.f);
	_averageFindingFood = sf::seconds(2.f);

	_timePhase = generateTimePhase(_averageEating);
	_beginPhase.restart();

	launchAnimation(WAIT);
}


void Antilope::beginIdle() {
	_lineOfSight = _lineOfSightStandard;
	_aStatus = IDLE;
	_speed = 0.f;
	_moving = false;
	launchAnimation(WAIT);
}

void Antilope::beginFleeing() {
	_lineOfSight = _lineOfSightStandard * 1.2;
	_aStatus = FLEEING;
	_speed = _speedRunning;
	_moving = true;
	launchAnimation(RUN);
}

void Antilope::beginRecovering() {
	_lineOfSight = _lineOfSightStandard * 1.1;
	_aStatus = RECOVERING;
	_speed = _speedWalking;
	_moving = true;
	launchAnimation(WALK);
}

sf::Time Antilope::generateTimePhase(sf::Time average) const {
	return 	average +
					sf::seconds(RANDOMF * average.asSeconds() * 0.8f)
					- sf::seconds(average.asSeconds() * 0.4f);
}

BoidsInfo Antilope::getInfoFromNeighbors(const std::unordered_set<igMovingElement*>& neighbors) const {
	BoidsInfo res;
	float distance;
	res.minRepDst = _repulsionRadius;

	for (auto it = neighbors.begin(); it != neighbors.end(); it++) {
		if ((*it) != this) {
			if (dynamic_cast<Antilope*>(*it)) {
				distance = vu::norm(_pos - (*it)->getPos());

				if (distance < _repulsionRadius) {
					if (distance < res.minRepDst) {
						res.closestRep = (*it)->getPos();
						res.minRepDst = distance;
					}
				}

				else if (distance < _orientationRadius) {
					res.sumOfDirs += (*it)->getDirection();
					res.nbDir++;
				}

				else if (distance < _attractionRadius) {
					res.sumPosAttract += (*it)->getPos();
					res.nbAttract++;
				}
			}

			else if (dynamic_cast<Lion*>((*it))) {
				distance = vu::norm(_pos - (*it)->getPos());

				if (distance < _lineOfSight) {
					res.sumPosFlee += (*it)->getPos();
					res.nbFlee++;
				}
			}
		}
	}

	return res;
}

void Antilope::reactWhenIdle(const BoidsInfo& info) {
	if (info.nbFlee != 0)
		beginFleeing();

	if (info.nbAttract != 0 && info.nbAttract <= 2) {
		_direction = info.sumPosAttract / (float) info.nbAttract - _pos;
		_direction /= vu::norm(_direction);
	}

	else if (_beginPhase.getElapsedTime() > _timePhase) {
		if (_moving) {
			_speed = 0.f;
			_moving = false;
			launchAnimation(WAIT);
			_timePhase = generateTimePhase(_averageEating);
			_beginPhase.restart();
		}

		else {
			if (info.minRepDst != _repulsionRadius) { // There is someone inside the repulsion radius
				_direction = _pos - info.closestRep;
				_direction /= vu::norm(_direction);
			}

			else {
				float theta = RANDOMF * 2.f * M_PI;
				_direction.x = cos(theta);
				_direction.y = sin(theta);
			}

			_speed = _speedWalking;
			_moving = true;
			launchAnimation(WALK);
			_timePhase = generateTimePhase(_averageFindingFood);
			_beginPhase.restart();
		}
	}
}

void Antilope::reactWhenFleeing(const BoidsInfo& info) {
	// Take into account the closest elements
	if (info.minRepDst != _repulsionRadius) {
		_direction = _pos - info.closestRep;
		_direction /= vu::norm(_direction);
	}

	else if (info.nbDir != 0) {
		sf::Vector2f zbla = info.sumOfDirs / (float) info.nbDir;
		if (zbla.x != 0 && zbla.y != 0) {
			_direction = info.sumOfDirs / (float) info.nbDir;
			_direction /= vu::norm(_direction);
		}

		else {
			_direction.x = 1.f;
			_direction.y = 0.f;
		}
	}

	else if (info.nbAttract != 0) {
		_direction = info.sumPosAttract / (float) info.nbAttract - _pos;
		_direction /= vu::norm(_direction);
	}

	else if (info.nbFlee != 0) {
		_direction = _pos - info.sumPosFlee / (float) info.nbFlee;
		_direction /= vu::norm(_direction);
	}

	// Take into account the hunter for half as much
	if (info.nbFlee != 0) {
		_direction = _pos - info.sumPosFlee / (float) info.nbFlee;
		_direction /= vu::norm(_direction);
	}

	else {
		beginRecovering();
		_timePhase = generateTimePhase(_averageRecovering);
		_beginPhase.restart();
	}
}

void Antilope::reactWhenRecovering(const BoidsInfo& info) {
	if (info.nbFlee != 0)
		beginFleeing();

	else if (_beginPhase.getElapsedTime() > _timePhase) {
		beginIdle();
		_beginPhase.restart();
	}

	else if (info.minRepDst != _repulsionRadius &&
			(_bStatus == REPULSION || vu::norm(_pos-info.closestRep) < _repulsionRadius * 0.8) ) {
		_direction = _pos - info.closestRep;
		_direction /= vu::norm(_direction);
		_bStatus = REPULSION;
	}

	else if (info.nbDir != 0) { // TODO : Hysteresis
		sf::Vector2f zbla = info.sumOfDirs / (float) info.nbDir;
		if (zbla.x != 0 && zbla.y != 0) {
			_direction = info.sumOfDirs / (float) info.nbDir;
			_direction /= vu::norm(_direction);
		}

		else {
			_direction.x = 1.f;
			_direction.y = 0.f;
		}

		_bStatus = ORIENTATION;
	}

	else if (info.nbAttract != 0 &&
			(_bStatus == ATTRACTION || vu::norm(info.sumPosAttract / (float) info.nbAttract - _pos) < _attractionRadius * 0.8) ) {
		_direction = info.sumPosAttract / (float) info.nbAttract - _pos;
		_direction /= vu::norm(_direction);
		_bStatus = ATTRACTION;
	}
}

void Antilope::updateState(const std::unordered_set<igMovingElement*>& neighbors) {
	BoidsInfo info = getInfoFromNeighbors(neighbors);

	switch (_aStatus) {
		case IDLE:
			reactWhenIdle(info);
			break;

		case FLEEING:
			reactWhenFleeing(info);
			break;

		case RECOVERING:
			reactWhenRecovering(info);
			break;
	}
}
