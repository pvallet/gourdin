#pragma once

#include <SFML/System.hpp>
#include "igMovingElement.h"

enum AntilopeStatus {IDLE, FLEEING, RECOVERING};

class Antilope : public igMovingElement {
public:
	Antilope(sf::Vector2f position, AnimationManager _graphics);
	virtual MovingType getMovingType() const {return PREY;}

	void updateState(std::vector<igElement*> neighbors);

	void beginIdle();
	void beginFleeing();
	void beginRecovering();

	inline float getLineOfSight() const {return lineOfSight;}
private:
	sf::Time generateTimePhase(sf::Time average) const;

	float lineOfSight;

	float repulsionRadius; // r < o < a
	float orientationRadius;
	float attractionRadius;

	float speedWalking;
	float speedRunning;

	AntilopeStatus status;

	sf::Time averageRecovering;
	sf::Time averageEating;
	sf::Time averageFindingFood;
	sf::Time timePhase;
	sf::Clock beginPhase;
};

