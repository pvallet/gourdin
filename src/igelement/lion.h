#pragma once

#include <list>

#include "controllable.h"
#include "clock.h"

enum LionStatus {WAITING, WALKING, RUNNING, ATTACKING, CHASING};

class Lion : public Controllable {
public:
	Lion(glm::vec2 position, AnimationManager _graphics, const TerrainGeometry& terrainGeometry);

	virtual void update(int msElapsed);
	// React to the environment
	virtual void updateState(const std::list<igMovingElement*>& neighbors);

	virtual void stop();
	void beginRunning();
	void beginWalking();
	void beginAttacking();
	void beginChasing();

	virtual void setTarget(glm::vec2 t, ANM_TYPE anim);
	inline float getHunger() const {return _hunger;}

private:

	float _hunger; // Percentage
	float _speedWalking;
	float _speedRunning;
	float _rangeAttack;
	float _rangeChase;

	LionStatus _status;

	igMovingElement* _prey;
	Clock _beginAttack;
	const int _msAnimAttack;
};
