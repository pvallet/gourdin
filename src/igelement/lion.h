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
	inline float getStamina() const {return _stamina;}
	inline bool isRunning() const {return _status == RUNNING;}

	void kill(std::vector<igElement*> neighbors);

private:
	float _stamina; // Percentage
	float _catchBreathSpeed; // Percents per second
	float _loseBreathSpeed;
	float _speedWalking;
	float _speedRunning;
	float _range;

	LionStatus _status;

	igMovingElement* _prey;
	sf::Clock _beginAttack;
	sf::Time _animAttack;
};
