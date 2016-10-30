#include "antilope.h"
#include "vecUtils.h"
#include "utils.h"
#include <cmath>
#include <iostream>

Antilope::Antilope(sf::Vector2f position, AnimationManager graphics) :
	igMovingElement(position, graphics),
	_lineOfSightStandard(50.),
	_repulsionRadius(8.),
	_orientationRadius(15.),
	_attractionRadius(50.),
	_speedWalking(5.),
	_speedRunning(11.),
	_aStatus(IDLE),
	_bStatus(ORIENTATION) {

	_size *= 5.f;
	_lineOfSight = _lineOfSightStandard;

	_averageRecovering = sf::seconds(3.f);
	_averageEating = sf::seconds(7.f);
	_averageFindingFood = sf::seconds(2.f);

	_timePhase = generateTimePhase(_averageEating);
	_beginPhase.restart();
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
					sf::seconds(random() * average.asSeconds() * 0.8f)
					- sf::seconds(average.asSeconds() * 0.4f);
}

void Antilope::updateState(std::vector<igMovingElement*> neighbors) {
	// Get info

	sf::Vector2f closestRep;
	sf::Vector2f baryAttract, baryFlee, meanDir;
	int nbDir = 0;
	int nbAttract = 0;
	int nbFlee = 0;
	float distance;
	float minRepDst = _repulsionRadius;

	for (unsigned int i = 0 ; i < neighbors.size() ; i++) {
		if (neighbors[i] != this) {
			if (dynamic_cast<Antilope*>(neighbors[i])) {
				distance = vu::norm(_pos - neighbors[i]->getPos());

				if (distance < _repulsionRadius) {
					if (distance < minRepDst) {
						closestRep = neighbors[i]->getPos();
						minRepDst = distance;
					}
				}

				else if (distance < _orientationRadius) {
					meanDir += neighbors[i]->getDirection();
					nbDir++;
				}

				else if (distance < _attractionRadius) {
					baryAttract += neighbors[i]->getPos();
					nbAttract++;
				}
			}

			else if (dynamic_cast<Antilope*>(neighbors[i])) {
				distance = vu::norm(_pos - neighbors[i]->getPos());

				if (distance < _lineOfSight) {
					baryFlee += neighbors[i]->getPos();
					nbFlee++;
				}
			}
		}
	}

	// Interpret the info

	switch (_aStatus) {
		case IDLE:
			if (nbFlee != 0)
				beginFleeing();

			if (nbAttract != 0 && nbAttract <= 2) {
				_direction = baryAttract / (float) nbAttract - _pos;
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
					if (minRepDst != _repulsionRadius) { // There is someone inside the repulsion radius
						_direction = _pos - closestRep;
						_direction /= vu::norm(_direction);
					}

					else {
						float theta = RANDOMF * 2. * M_PI;
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

		break;

		case FLEEING: {
			// Take into account the closest elements
			if (minRepDst != _repulsionRadius) {
				_direction = _pos - closestRep;
				_direction /= vu::norm(_direction);
			}

			else if (nbDir != 0) {
				sf::Vector2f zbla = meanDir / (float) nbDir;
				if (zbla.x != 0 && zbla.y != 0) {
					_direction = meanDir / (float) nbDir;
					_direction /= vu::norm(_direction);
				}

				else {
					_direction.x = 1.;
					_direction.y = 0.;
				}
			}

			else if (nbAttract != 0) {
				_direction = baryAttract / (float) nbAttract - _pos;
				_direction /= vu::norm(_direction);
			}

			else if (nbFlee != 0) {
				_direction = _pos - baryFlee / (float) nbFlee;
				_direction /= vu::norm(_direction);
			}

			// Take into account the hunter for half as much
			if (nbFlee != 0) {
				_direction = _pos - baryFlee / (float) nbFlee;
				_direction /= vu::norm(_direction);
			}

			else {
				beginRecovering();
				_timePhase = generateTimePhase(_averageRecovering);
				_beginPhase.restart();
			}

		}
		break;

		case RECOVERING: {
			if (nbFlee != 0)
				beginFleeing();

			else if (_beginPhase.getElapsedTime() > _timePhase) {
				beginIdle();
				_beginPhase.restart();
			}

			else if (minRepDst != _repulsionRadius &&
					(_bStatus == REPULSION || vu::norm(_pos-closestRep) < _repulsionRadius * 0.8) ) {
				_direction = _pos - closestRep;
				_direction /= vu::norm(_direction);
				_bStatus = REPULSION;
			}

			else if (nbDir != 0) { // TODO : Hysteresis
				sf::Vector2f zbla = meanDir / (float) nbDir;
				if (zbla.x != 0 && zbla.y != 0) {
					_direction = meanDir / (float) nbDir;
					_direction /= vu::norm(_direction);
				}

				else {
					_direction.x = 1.;
					_direction.y = 0.;
				}

				_bStatus = ORIENTATION;
			}

			else if (nbAttract != 0 &&
					(_bStatus == ATTRACTION || vu::norm(baryAttract / (float) nbAttract - _pos) < _attractionRadius * 0.8) ) {
				_direction = baryAttract / (float) nbAttract - _pos;
				_direction /= vu::norm(_direction);
				_bStatus = ATTRACTION;
			}
		}
		break;
	}

}
