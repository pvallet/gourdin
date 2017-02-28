#pragma once

#include <unordered_set>

#include "controllable.h"

enum LionStatus {WAITING, WALKING, RUNNING, ATTACKING};

class Lion : public Controllable {
public:
	Lion(sf::Vector2f position, AnimationManager _graphics);
	virtual ~Lion() {}

	inline Animals getAnimalType() const {return LION;}

	virtual void update(sf::Time elapsed);

	virtual void stop();
	void beginRunning();
	void beginWalking();
	void beginAttacking();

	virtual void setTarget(sf::Vector2f t);
	inline float getStamina() const {return _stamina;}
	inline bool isRunning() const {return _status == RUNNING;}

	void kill(const std::unordered_set<igMovingElement*>& neighbors);

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
