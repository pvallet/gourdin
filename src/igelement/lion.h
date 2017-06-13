#pragma once

#include <list>

#include "controllable.h"
#include "clock.h"

enum LionStatus {WAITING, WALKING, RUNNING, ATTACKING, CHASING};

class Lion : public Controllable {
public:
	Lion(glm::vec2 position, AnimationManager _graphics, const TerrainGeometry& terrainGeometry);
	virtual ~Lion() {}

	virtual void update(int msElapsed);
	// React to the environment
	virtual void updateState(const std::list<igMovingElement*>& neighbors);

	virtual void stop();
	void beginRunning();
	void beginWalking();
	void beginAttacking();
	void beginChasing();

	virtual void setTarget(glm::vec2 t, ANM_TYPE anim);
	inline float getStamina() const {return _stamina;}
	inline bool isRunning() const {return _status == RUNNING;}

private:
	float _stamina; // Percentage
	float _catchBreathSpeed; // Percents per second
	float _loseBreathSpeed;
	float _speedWalking;
	float _speedRunning;
	float _rangeAttack;
	float _rangeChase;

	LionStatus _status;

	igMovingElement* _prey;
	Clock _beginAttack;
	int _msAnimAttack;
};
