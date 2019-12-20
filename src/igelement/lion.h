#pragma once

#include <list>

#include "controllable.h"
#include "clock.h"

enum LionStatus {RESTING, ROAMING, ATTACKING, CHASING, NB_LION_STATUSES};

class Lion : public Controllable {
public:
	Lion(glm::vec2 position, AnimationManager _graphics, const TerrainGeometry& terrainGeometry);

	virtual void update(int msElapsed);
	// React to the environment
	virtual void updateState(const std::list<igMovingElement*>& neighbors);

	virtual void stop();
	void beginRoaming();
	void beginAttacking();
	void beginChasing();

	inline float getHunger() const {return _hunger;}

private:
	typedef Controllable Super;

	std::array<float, NB_LION_STATUSES> _hungerRates;

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
