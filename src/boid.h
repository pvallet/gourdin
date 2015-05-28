#pragma once

#include "controllable.h"

class Boid : public Controllable {
public:
	Boid(sf::Vector2f position, std::string filename);

	void updateState(std::vector<igElement> neighbors);

	inline float getEyesightRange() const {return eyesightRange;}
private:
	float eyesightRange;

	float repulsionRadius; // r < o < a
	float orientationRadius;
	float attractionRadius;
};

