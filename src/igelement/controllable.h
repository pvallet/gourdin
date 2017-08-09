#pragma once

#include <string>
#include "igMovingElement.h"

class Controllable : public igMovingElement {
public:
	Controllable(glm::vec2 position, AnimationManager _graphics, const TerrainGeometry& terrainGeometry);

	virtual void update(int msElapsed);
	virtual void setTarget(glm::vec2 t, ANM_TYPE anim = WALK);
	void setMovingDirection(glm::vec2 direction);
	virtual void stop();

	inline void setProjectedVertices(std::array<float,12> nVertices) {_projectedVertices = nVertices;}

	glm::uvec4 getScreenRect() const;

protected:
	glm::vec2 _target;

	bool _alwaysInSameDirection;

	std::array<float, 12> _projectedVertices;
};
