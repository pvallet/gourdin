#include "lion.h"
#include "vecUtils.h"
#include <iostream>

Lion::Lion(sf::Vector2<double> position, AnimationManager _graphics) :
	Controllable(position, _graphics),
	stamina(100),
	catchBreathSpeed(25.),
	loseBreathSpeed(10.),
	speedWalking(7.),
	speedRunning(15.),
	range(5.),
	status(WAITING) {

	height = 5.f;
	speed = speedWalking;
	animAttack = graphics.getAnimationTime(ATTACK);
}

void Lion::update(sf::Time elapsed, float theta) {
	switch(status) {
		case RUNNING:
			stamina -= elapsed.asSeconds() * loseBreathSpeed;
			if (stamina <= 0.f) {
				stamina = 0.f;
				beginWalking();
			}
			break;

		case ATTACKING:
			target = prey->getPos();
			direction = prey->getPos() - pos;
			direction /= vu::norm(direction);
			speed = prey->getSpeed() - 1.f;

			if (beginAttack.getElapsedTime() >= animAttack) {
				prey->die();
				stop();
			}
			break;

		default:
			stamina += elapsed.asSeconds() * catchBreathSpeed;
			if (stamina >= 100.f)
				stamina = 100.f;
			break;
	}

	Controllable::update(elapsed, theta);
}

void Lion::stop() {
	igMovingElement::stop();
	status = WAITING;
}

void Lion::beginRunning() {
	if (stamina > 0.) {
		moving = true;
		status = RUNNING;
		speed = speedRunning;
		launchAnimation(RUN);
	}
}

void Lion::beginWalking() {
	moving = true;
	status = WALKING;
	speed = speedWalking;
	launchAnimation(WALK);
}

void Lion::beginAttacking() {
	if (status != ATTACKING) {
		status = ATTACKING;
		speed = 0.f;
		launchAnimation(ATTACK);
		beginAttack.restart();
	}
}

void Lion::setTarget(sf::Vector2<double> t) {
	Controllable::setTarget(t);
	if (status == WAITING) {
		beginWalking();
	}
}

void Lion::kill(std::vector<igElement*> neighbors) {
	float distance;
	igMovingElement* igM;
	igMovingElement* closest = NULL;
	float nearestDist = range;

	for (unsigned int i = 0 ; i < neighbors.size() ; i++) {
		if (neighbors[i]->getAbstractType() != igE && neighbors[i] != this) {
			igM = (igMovingElement*) neighbors[i];

			if (igM->getMovingType() == PREY) {
				distance = vu::norm(pos - igM->getPos());

				if (distance < range) {
					if (distance < nearestDist) {
						nearestDist = distance;
						closest = igM;
					}
				}
			}
		}
	}

	if (nearestDist != range && !closest->isDead()) {
		beginAttacking();
		prey = closest;
	}
}
