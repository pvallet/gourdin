#pragma once

#include <SFML/System.hpp>

#include <list>

#include "igMovingElement.h"

enum AntilopeStatus {IDLE, FLEEING, RECOVERING};
enum BoidStatus {REPULSION, ORIENTATION, ATTRACTION};

struct BoidsInfo {
	sf::Vector2f closestRep;
	sf::Vector2f sumPosAttract, sumPosFlee, sumOfDirs;
	int nbDir = 0;
	int nbAttract = 0;
	int nbFlee = 0;
	float minRepDst;
};

class Antilope : public igMovingElement {
public:
	Antilope(sf::Vector2f position, AnimationManager graphics, const TerrainGeometry& terrainGeometry);
	virtual ~Antilope() {}

	// React to the environment
	virtual void updateState(const std::list<igMovingElement*>& neighbors);

	void beginIdle();
	void beginFleeing();
	void beginRecovering();

	inline float getLineOfSight() const {return _lineOfSight;}

protected:
	void setDirection(sf::Vector2f direction);

private:
	sf::Time generateTimePhase(sf::Time average) const;

	BoidsInfo getInfoFromNeighbors(const std::list<igMovingElement*>& neighbors) const;
	void reactWhenIdle      (const BoidsInfo& info);
	void reactWhenFleeing   (const BoidsInfo& info);
	void reactWhenRecovering(const BoidsInfo& info);

	float _lineOfSight;
	float _lineOfSightStandard; // * 1 for normal line of sight, * 1.1 for hysteresis

	float _repulsionRadius; // r < o < a
	float _orientationRadius;
	float _attractionRadius;

	float _speedWalking;
	float _speedRunning;

	AntilopeStatus _aStatus;
	BoidStatus _bStatus; // To control hysteresis
	bool _moving;

	const sf::Time _averageRecovering;
	const sf::Time _averageEating;
	const sf::Time _averageFindingFood;
	sf::Time _timePhase;
	sf::Clock _beginPhase;

	sf::Clock _lastDirectionChange;
	const sf::Time _averageTimeBeforeChangingDir;
	sf::Time _timeBeforeChangingDir;
};
