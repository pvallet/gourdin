#pragma once

#include "controllable.h"

enum LionStatus {WAITING, WALKING, RUNNING, ATTACKING};

class Lion : public Controllable {
public:
	Lion(sf::Vector2<double> position, AnimationManager _graphics);

	virtual void update(sf::Time elapsed, float theta);

	virtual void stop();
	void beginRunning();
	void beginWalking();
	void beginAttacking();

	virtual void setTarget(sf::Vector2<double> t);
	virtual MovingType getMovingType() const {return HUNTER;}
	inline float getStamina() const {return stamina;}
	inline bool isRunning() const {return status == RUNNING;}

	void kill(std::vector<igElement*> neighbors);

private:
	float stamina; // Percentage
	float catchBreathSpeed; // Percents per second
	float loseBreathSpeed;
	float speedWalking;
	float speedRunning;
	float range;

	LionStatus status;

	igMovingElement* prey;
	sf::Clock beginAttack;
	sf::Time animAttack;
};

