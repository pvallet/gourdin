#pragma once

#include <list>

#include "chronometer.h"
#include "igMovingElement.h"

enum class AntilopeStatus {IDLE, FLEEING, RECOVERING};
enum class BoidStatus {REPULSION, ORIENTATION, ATTRACTION};

struct BoidsInfo {
	glm::vec2 closestRep, closestFlee;
	glm::vec2 sumPosAttract, sumPosFlee, sumOfDirs;
	int nbDir = 0;
	int nbAttract = 0;
	int nbFlee = 0;
	float minRepDst, minFleeDst;
};

class Antilope : public igMovingElement {
public:
	Antilope(glm::vec2 position, AnimationManager graphics, const TerrainGeometry& terrainGeometry);

	// React to the environment
	virtual void updateState(const std::list<igMovingElement*>& neighbors);

	void beginIdle();
	void beginFleeing();
	void beginRecovering();

	static float getStandardLineOfSight() {return _standardLineOfSight;}

protected:
	void setDirection(glm::vec2 direction);

private:
	int generateTimePhase(int msAverage) const;

	BoidsInfo getInfoFromNeighbors(const std::list<igMovingElement*>& neighbors) const;
	void reactWhenIdle      (const BoidsInfo& info);
	void reactWhenFleeing   (const BoidsInfo& info);
	void reactWhenRecovering(const BoidsInfo& info);

	static const float _standardLineOfSight;

	float _lineOfSight; // Changes the standard line of sight to add hysteresis
	float _panicFleeRadius;

	float _repulsionRadius; // r < o < a
	float _orientationRadius;
	float _attractionRadius;

	float _speedWalking;
	float _speedRunning;

	AntilopeStatus _antilopeStatus;
	BoidStatus _boidStatus;
	bool _moving;

	const int _msAverageRecovering;
	const int _msAverageEating;
	const int _msAverageFindingFood;
	Chronometer _currentPhase;

	Chronometer _noDirectionChange;
	const int _msAverageTimeBeforeChangingDir;
	int _timesChangedDir;
};
