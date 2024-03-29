#pragma once

#include <list>
#include <stddef.h> // size_t

#include "controllable.h"
#include "clock.h"

enum class LionStatus {WAITING, WALKING, RUNNING, ATTACKING, CHASING};

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
	inline float getStamina() const {return _stamina;}
	inline bool isRunning() const {return _status == LionStatus::RUNNING;}

	inline static size_t getNbKilled() {return _nbKilled;}
	inline static void resetNbKilled() {_nbKilled = 0;}

private:
	static size_t _nbKilled;

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
	const int _msAnimAttack;
};
