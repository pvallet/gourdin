#include "antilope.h"
#include "vecUtils.h"
#include "utils.h"
#include <cmath>
#include <iostream>

Antilope::Antilope(sf::Vector2<double> position, AnimationManager _graphics) :
	igMovingElement(position, _graphics),
	lineOfSightStandard(50.),
	repulsionRadius(8.),
	orientationRadius(15.),
	attractionRadius(50.),
	speedWalking(5.),
	speedRunning(11.),
	aStatus(IDLE),
	bStatus(ORIENTATION) {

	height = 5.;
	lineOfSight = lineOfSightStandard;

	averageRecovering = sf::seconds(3.f);
	averageEating = sf::seconds(7.f);
	averageFindingFood = sf::seconds(2.f);

	timePhase = generateTimePhase(averageEating);
	beginPhase.restart();
}


void Antilope::beginIdle() {
	lineOfSight = lineOfSightStandard;
	aStatus = IDLE;
	speed = 0.f;
	moving = false;
	launchAnimation(WAIT);
}

void Antilope::beginFleeing() {
	lineOfSight = lineOfSightStandard * 1.2;
	aStatus = FLEEING;
	speed = speedRunning;
	moving = true;
	launchAnimation(RUN);
}

void Antilope::beginRecovering() {
	lineOfSight = lineOfSightStandard * 1.1;
	aStatus = RECOVERING;
	speed = speedWalking;
	moving = true;
	launchAnimation(WALK);
}

sf::Time Antilope::generateTimePhase(sf::Time average) const {
	return 	average + 
			sf::seconds(randomF() * average.asSeconds() * 0.8f)
			- sf::seconds(average.asSeconds() * 0.4f);
}

void Antilope::updateState(std::vector<igElement*> neighbors) {
	// Get info

	sf::Vector2<double> closestRep;
	sf::Vector2<double> baryAttract, baryFlee, meanDir;
	int nbDir = 0;
	int nbAttract = 0;
	int nbFlee = 0;
	double distance;
	double minRepDst = repulsionRadius;
	igMovingElement* igM;

	for (unsigned int i = 0 ; i < neighbors.size() ; i++) {
		if (neighbors[i]->getAbstractType() != igE && neighbors[i] != this) {
			igM = (igMovingElement*) neighbors[i];

			if (igM->getMovingType() == PREY) {
				distance = vu::norm(pos - igM->getPos());

				if (distance < repulsionRadius) {
					if (distance < minRepDst) {
						closestRep = igM->getPos();
						minRepDst = distance;
					}
				}

				else if (distance < orientationRadius) {
					meanDir += igM->getDirection();
					nbDir++;
				}

				else if (distance < attractionRadius) {
					baryAttract += igM->getPos();
					nbAttract++;
				}
			}

			else if (igM->getMovingType() == HUNTER) {
				distance = vu::norm(pos - igM->getPos());

				if (distance < lineOfSight) {
					baryFlee += igM->getPos();
					nbFlee++;
				}
			}
		}
	}

	// Interpret the info

	switch (aStatus) {
		case IDLE:
			if (nbFlee != 0)
				beginFleeing();

			if (nbAttract != 0 && nbAttract <= 2) {
				direction = baryAttract / (double) nbAttract - pos;
				direction /= vu::norm(direction);
			}

			else if (beginPhase.getElapsedTime() > timePhase) {
				if (moving) {
					speed = 0.f;
					moving = false;
					launchAnimation(WAIT);
					timePhase = generateTimePhase(averageEating);
					beginPhase.restart();
				}

				else {
					if (minRepDst != repulsionRadius) { // There is someone inside the repulsion radius
						direction = pos - closestRep;
						direction /= vu::norm(direction);
					}

					else {
						float theta = randomD() * 2. * M_PI;
						direction.x = cos(theta);
						direction.y = sin(theta);
					}

					speed = speedWalking;
					moving = true;
					launchAnimation(WALK);
					timePhase = generateTimePhase(averageFindingFood);
					beginPhase.restart();
				}
			}

		break;

		case FLEEING: {
			// Take into account the closest elements
			if (minRepDst != repulsionRadius) {
				direction = pos - closestRep;
				direction /= vu::norm(direction);
			}

			else if (nbDir != 0) {
				sf::Vector2<double> zbla = meanDir / (double) nbDir;
				if (zbla.x != 0 && zbla.y != 0) {
					direction = meanDir / (double) nbDir;
					direction /= vu::norm(direction);
				}

				else {
					direction.x = 1.;
					direction.y = 0.;
				}
			}

			else if (nbAttract != 0) {
				direction = baryAttract / (double) nbAttract - pos;
				direction /= vu::norm(direction);
			}

			else if (nbFlee != 0) {
				direction = pos - baryFlee / (double) nbFlee;
				direction /= vu::norm(direction);
			}

			// Take into account the hunter for half as much
			if (nbFlee != 0) {
				direction = pos - baryFlee / (double) nbFlee;
				direction /= vu::norm(direction);
			}

			else {
				beginRecovering();
				timePhase = generateTimePhase(averageRecovering);
				beginPhase.restart();
			}

		}
		break;

		case RECOVERING: {
			if (nbFlee != 0)
				beginFleeing();

			else if (beginPhase.getElapsedTime() > timePhase) {
				beginIdle();
				beginPhase.restart();
			}

			else if (minRepDst != repulsionRadius &&
					(bStatus == REPULSION || vu::norm(pos-closestRep) < repulsionRadius * 0.8) ) {
				direction = pos - closestRep;
				direction /= vu::norm(direction);
				bStatus = REPULSION;
			}

			else if (nbDir != 0) { // TODO : Hysteresis
				sf::Vector2<double> zbla = meanDir / (double) nbDir;
				if (zbla.x != 0 && zbla.y != 0) {
					direction = meanDir / (double) nbDir;
					direction /= vu::norm(direction);
				}

				else {
					direction.x = 1.;
					direction.y = 0.;
				}

				bStatus = ORIENTATION;
			}

			else if (nbAttract != 0 && 
					(bStatus == ATTRACTION || vu::norm(baryAttract / (double) nbAttract - pos) < attractionRadius * 0.8) ) {
				direction = baryAttract / (double) nbAttract - pos;
				direction /= vu::norm(direction);
				bStatus = ATTRACTION;
			}
		}
		break;
	}
	
}