#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "igMovingElement.h"

class Controllable : public igMovingElement {
public:
	Controllable(sf::Vector2f position, AnimationManager _graphics, const TerrainGeometry& terrainGeometry);
	virtual ~Controllable() {}

	virtual void update(sf::Time elapsed);
	virtual void setTarget(sf::Vector2f t);
	virtual void stop();

	inline void setProjectedVertices(std::array<float,12> nVertices) {_projectedVertices = nVertices;}

	sf::IntRect getScreenCoord() const;

protected:
	sf::Vector2f _target;

	std::array<float, 12> _projectedVertices;
};
