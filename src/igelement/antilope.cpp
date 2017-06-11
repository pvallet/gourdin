#include "antilope.h"

#include <cmath>
#include <iostream>

#include "lion.h"
#include "utils.h"
#include "vecUtils.h"

Antilope::Antilope(glm::vec2 position, AnimationManager graphics, const TerrainGeometry& terrainGeometry) :
	igMovingElement(position, graphics, terrainGeometry),
	_lineOfSightStandard(50.f),
	_repulsionRadius(8.f),
	_orientationRadius(15.f),
	_attractionRadius(50.f),
	_speedWalking(7.f),
	_speedRunning(15.f),
	_aStatus(IDLE),
	_bStatus(ORIENTATION),
	_moving(false),
	_averageRecovering(sf::seconds(3.f)),
	_averageEating(sf::seconds(7.f)),
	_averageFindingFood(sf::seconds(2.f)),
	_averageTimeBeforeChangingDir(sf::milliseconds(500)),
	_timeBeforeChangingDir(sf::Time::Zero) {

	_lineOfSight = _lineOfSightStandard;

	_timePhase = generateTimePhase(_averageEating);
	_beginPhase.restart();
}


void Antilope::beginIdle() {
	_lineOfSight = _lineOfSightStandard;
	_aStatus = IDLE;
	_speed = 0.f;
	_moving = false;
	launchAnimation(WAIT);
	_beginPhase.restart();
}

void Antilope::beginFleeing() {
	_lineOfSight = _lineOfSightStandard * 1.2;
	_aStatus = FLEEING;
	_speed = _speedRunning;
	_moving = true;
	launchAnimation(RUN);
	_beginPhase.restart();
}

void Antilope::beginRecovering() {
	_lineOfSight = _lineOfSightStandard * 1.1;
	_aStatus = RECOVERING;
	_speed = _speedWalking;
	_moving = true;
	launchAnimation(WALK);
	_timePhase = generateTimePhase(_averageRecovering);
	_beginPhase.restart();
}

sf::Time Antilope::generateTimePhase(sf::Time average) const {
	return 	average +
					sf::seconds(RANDOMF * average.asSeconds() * 0.8f)
					- sf::seconds(average.asSeconds() * 0.4f);
}

BoidsInfo Antilope::getInfoFromNeighbors(const std::list<igMovingElement*>& neighbors) const {
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

	if (info.nbAttract != 0 && info.nbAttract <= 2)
		setDirection(info.sumPosAttract / (float) info.nbAttract - _pos);

	else if (_beginPhase.getElapsedTime() > _timePhase) {
		if (_moving) {
			_speed = 0.f;
			_moving = false;
			launchAnimation(WAIT);
			_timePhase = generateTimePhase(_averageEating);
			_beginPhase.restart();
		}

		else {
			if (info.minRepDst != _repulsionRadius) // There is someone inside the repulsion radius
				setDirection(_pos - info.closestRep);

			else {
				float theta = RANDOMF * 2.f * M_PI;
				setDirection(glm::vec2(cos(theta), sin(theta)));
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
	if (info.nbFlee != 0)
		setDirection(_pos - info.sumPosFlee / (float) info.nbFlee);

	else if (info.minRepDst != _repulsionRadius)
		setDirection(_pos - info.closestRep);

	else if (info.nbDir != 0) {
		glm::vec2 zbla = info.sumOfDirs / (float) info.nbDir;
		if (zbla.x != 0 && zbla.y != 0)
			setDirection(info.sumOfDirs / (float) info.nbDir);

		else
			setDirection(glm::vec2(1,0));
	}

	else if (info.nbAttract != 0)
		setDirection(info.sumPosAttract / (float) info.nbAttract - _pos);

	// Take into account the hunter for half as much
	if (info.nbFlee != 0)
		setDirection(_pos - info.sumPosFlee / (float) info.nbFlee);

	else
		beginRecovering();
}

void Antilope::reactWhenRecovering(const BoidsInfo& info) {
	if (info.nbFlee != 0)
		beginFleeing();

	else if (_beginPhase.getElapsedTime() > _timePhase)
		beginIdle();

	else if (info.minRepDst != _repulsionRadius &&
			(_bStatus == REPULSION || vu::norm(_pos-info.closestRep) < _repulsionRadius * 0.8) ) {
		setDirection(_pos - info.closestRep);
		_bStatus = REPULSION;
	}

	else if (info.nbDir != 0) {
		glm::vec2 zbla = info.sumOfDirs / (float) info.nbDir;
		if (zbla.x != 0 && zbla.y != 0)
			setDirection(info.sumOfDirs / (float) info.nbDir);

		else
			setDirection(glm::vec2(1,0));

		_bStatus = ORIENTATION;
	}

	else if (info.nbAttract != 0 &&
			(_bStatus == ATTRACTION || vu::norm(info.sumPosAttract / (float) info.nbAttract - _pos) < _attractionRadius * 0.8) ) {
		setDirection(info.sumPosAttract / (float) info.nbAttract - _pos);
		_bStatus = ATTRACTION;
	}
}

void Antilope::updateState(const std::list<igMovingElement*>& neighbors) {
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

void Antilope::setDirection(glm::vec2 direction) {
	if (_lastDirectionChange.getElapsedTime() > _timeBeforeChangingDir) {
		igMovingElement::setDirection(direction);
		_lastDirectionChange.restart();
		_timeBeforeChangingDir = generateTimePhase(_averageTimeBeforeChangingDir);

		// if the antilope has been fleeing for a long time, it will try its luck
		// by going for a longer time towards the same direction
		if (_aStatus == FLEEING)
			_timeBeforeChangingDir += 0.5f * _beginPhase.getElapsedTime();
	}
}
