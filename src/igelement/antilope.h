#pragma once

#include <SFML/System.hpp>

#include "igMovingElement.h"

enum AntilopeStatus {IDLE, FLEEING, RECOVERING};
enum BoidStatus {REPULSION, ORIENTATION, ATTRACTION};

class Antilope : public igMovingElement {
public:
	Antilope(sf::Vector2f position, AnimationManager graphics);

	// React to the environment
	void updateState(std::vector<igMovingElement*> neighbors);

	void beginIdle();
	void beginFleeing();
	void beginRecovering();

	inline float getLineOfSight() const {return _lineOfSight;}
private:
	sf::Time generateTimePhase(sf::Time average) const;

	float _lineOfSight;
	float _lineOfSightStandard; // * 1 for normal line of sight, * 1.1 for hysteresis

	float _repulsionRadius; // r < o < a
	float _orientationRadius;
	float _attractionRadius;

	float _speedWalking;
	float _speedRunning;

	AntilopeStatus _aStatus;
	BoidStatus _bStatus; // To control hysteresis

	sf::Time _averageRecovering;
	sf::Time _averageEating;
	sf::Time _averageFindingFood;
	sf::Time _timePhase;
	sf::Clock _beginPhase;
};
