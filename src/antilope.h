#pragma once

#include <SFML/System.hpp>
#include "igMovingElement.h"

enum AntilopeStatus {IDLE, FLEEING, RECOVERING};
enum BoidStatus {REPULSION, ORIENTATION, ATTRACTION};

class Antilope : public igMovingElement {
public:
	Antilope(sf::Vector2<double> position, AnimationManager _graphics);
	virtual MovingType getMovingType() const {return PREY;}

	// React to the environment
	void updateState(std::vector<igElement*> neighbors);

	void beginIdle();
	void beginFleeing();
	void beginRecovering();

	inline float getLineOfSight() const {return lineOfSight;}
private:
	sf::Time generateTimePhase(sf::Time average) const;

	float lineOfSight;
	float lineOfSightStandard; // * 1 for normal line of sight, * 1.1 for hysteresis

	float repulsionRadius; // r < o < a
	float orientationRadius;
	float attractionRadius;

	float speedWalking;
	float speedRunning;

	AntilopeStatus aStatus;
	BoidStatus bStatus; // To control hysteresis

	sf::Time averageRecovering;
	sf::Time averageEating;
	sf::Time averageFindingFood;
	sf::Time timePhase;
	sf::Clock beginPhase;
};

