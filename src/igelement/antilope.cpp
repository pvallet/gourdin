#include "antilope.h"

#include <cmath>

#include "lion.h"
#include "utils.h"

const float Antilope::_standardLineOfSight = 60.f;

Antilope::Antilope(glm::vec2 position, AnimationManager graphics, const TerrainGeometry& terrainGeometry) :
	igMovingElement(position, graphics, terrainGeometry),
	_panicFleeRadius(20.f),
	_baseFleeRadius(30.f),
	_repulsionRadius(8.f),
	_orientationRadius(15.f),
	_attractionRadius(50.f),
	_speedWalking(7.f),
	_speedRunning(15.f),
	_boidStatus(ORIENTATION),
	_msAverageRecovering(3000),
	_msAverageEating(7000),
	_msAverageFindingFood(2000),
	_msAverageTimeBeforeChangingDir(500),
	_timesChangedDir(0) {

	beginIdle();

	_currentPhase.reset(generateTimePhase(_msAverageEating));
}


void Antilope::beginIdle() {
	if (_antilopeStatus != IDLE)
		_timesChangedDir = 0;
	_antilopeStatus = IDLE;
	_lineOfSight = _standardLineOfSight * 0.8;
	_speed = 0.f;
	_moving = false;
	launchAnimation(WAIT);
}

void Antilope::beginFleeing() {
	if (_antilopeStatus != FLEEING)
		_timesChangedDir = 0;
	_antilopeStatus = FLEEING;
	_lineOfSight = _standardLineOfSight;
	_speed = _speedRunning;
	_moving = true;
	launchAnimation(RUN);
}

void Antilope::beginRecovering() {
	if (_antilopeStatus != RECOVERING)
		_timesChangedDir = 0;
	_antilopeStatus = RECOVERING;
	_lineOfSight = _standardLineOfSight * 0.9;
	_speed = _speedWalking;
	_moving = true;
	launchAnimation(WALK);
	_currentPhase.reset(generateTimePhase(_msAverageRecovering));
}

int Antilope::generateTimePhase(int msAverage) const {
	return 	msAverage + RANDOMF * msAverage * 0.8f - msAverage * 0.4f;
}

BoidsInfo Antilope::getInfoFromNeighbors(const std::list<igMovingElement*>& neighbors) const {
	BoidsInfo res;
	float distance;
	res.minRepDst = _repulsionRadius;
	res.minFleeDst = _panicFleeRadius;

	for (auto it = neighbors.begin(); it != neighbors.end(); it++) {
		if ((*it) != this) {
			distance = glm::length(_pos - (*it)->getPos());

			if (dynamic_cast<Antilope*>(*it)) {

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

				if (distance < _baseFleeRadius) {
					if (distance < res.minFleeDst) {
						res.closestFlee = (*it)->getPos();
						res.minFleeDst = distance;
					}

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

	else if (!_currentPhase.isStillRunning()) {
		if (_moving) {
			_speed = 0.f;
			_moving = false;
			launchAnimation(WAIT);
			_currentPhase.reset(generateTimePhase(_msAverageEating));
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
			_currentPhase.reset(generateTimePhase(_msAverageFindingFood));
		}
	}
}

void Antilope::reactWhenFleeing(const BoidsInfo& info) {
	if (info.nbFlee != 0) {
		if (info.minFleeDst != _panicFleeRadius)
			setDirection(_pos - (info.closestFlee + info.sumPosFlee) / (float) (info.nbFlee+1));

		else
			setDirection(_pos - info.sumPosFlee / (float) info.nbFlee);
	}

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

	if (info.nbFlee == 0)
		beginRecovering();
}

void Antilope::reactWhenRecovering(const BoidsInfo& info) {
	if (info.nbFlee != 0)
		beginFleeing();

	else if (!_currentPhase.isStillRunning())
		beginIdle();

	else if (info.minRepDst != _repulsionRadius &&
			(_boidStatus == REPULSION || glm::length(_pos-info.closestRep) < _repulsionRadius * 0.8) ) {
		setDirection(_pos - info.closestRep);
		_boidStatus = REPULSION;
	}

	else if (info.nbDir != 0) {
		glm::vec2 zbla = info.sumOfDirs / (float) info.nbDir;
		if (zbla.x != 0 && zbla.y != 0)
			setDirection(info.sumOfDirs / (float) info.nbDir);

		else
			setDirection(glm::vec2(1,0));

		_boidStatus = ORIENTATION;
	}

	else if (info.nbAttract != 0 &&
			(_boidStatus == ATTRACTION || glm::length(info.sumPosAttract / (float) info.nbAttract - _pos) < _attractionRadius * 0.8) ) {
		setDirection(info.sumPosAttract / (float) info.nbAttract - _pos);
		_boidStatus = ATTRACTION;
	}
}

void Antilope::updateState(const std::list<igMovingElement*>& neighbors) {
	BoidsInfo info = getInfoFromNeighbors(neighbors);

	switch (_antilopeStatus) {
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
	if (!_noDirectionChange.isStillRunning()) {
		igMovingElement::setDirection(direction);
		int msTimeBeforeChangingDir = generateTimePhase(_msAverageTimeBeforeChangingDir);

		// if the antilope has been fleeing for a long time, it will try its luck
		// by going for a longer time towards the same direction
		if (_antilopeStatus == FLEEING)
			msTimeBeforeChangingDir += 200.f * _timesChangedDir;

		_timesChangedDir++;

		_noDirectionChange.reset(msTimeBeforeChangingDir);
	}
}
