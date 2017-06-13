#pragma once

#include <string>
#include "igMovingElement.h"

class Controllable : public igMovingElement {
public:
	Controllable(glm::vec2 position, AnimationManager _graphics, const TerrainGeometry& terrainGeometry);
	virtual ~Controllable() {}

	virtual void update(int msElapsed);
	virtual void setTarget(glm::vec2 t, ANM_TYPE anim = WALK);
	virtual void stop();

	inline void setProjectedVertices(std::array<float,12> nVertices) {_projectedVertices = nVertices;}

	glm::ivec4 getScreenRect() const;

protected:
	glm::vec2 _target;

	std::array<float, 12> _projectedVertices;
};
