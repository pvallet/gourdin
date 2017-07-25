#pragma once

#include <string>

#include "animationManager.h"
#include "igElement.h"
#include "terrainGeometry.h"

class igMovingElement : public igElement {
public:
	igMovingElement(glm::vec2 position, AnimationManager graphics, const TerrainGeometry& terrainGeometry);
	virtual ~igMovingElement() {}

	void launchAnimation (ANM_TYPE type);
	virtual void updateDisplay(int msElapsed, float theta); // Update sprite
	virtual void update(int msElapsed); // Update pos and inner statuses
	// React to the environment
	virtual void updateState(const std::list<igMovingElement*>& neighbors) {(void) neighbors;}
	virtual void die();
	virtual void stop();

	inline const TextureArray* getTexArray() const {return _graphics.getTexArray();}

	inline virtual float getMaxHeightFactor() const {return _graphics.getMaxHeightFactor();}
	inline glm::vec2 getDirection() const {return _direction;}
	inline float getSpeed() const {return _speed;}
	inline bool isDead() const {return _dead;}

protected:
	virtual void setDirection(glm::vec2 direction);

	float _speed; // Distance per second
	bool _dead;

	AnimationManager _graphics;

	const TerrainGeometry& _terrainGeometry;

private:
	// Normalized vector towards the target
	// It is private to guarantee a correct normalization
	glm::vec2 _direction;
};
