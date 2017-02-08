#pragma once

#include <SFML/System.hpp>

#include <map>
#include <string>
#include <vector>

#include "animationManagerInitializer.h"

class AnimationManager {
public:
	AnimationManager(const AnimationManagerInitializer& init);

	size_t launchAnimation(ANM_TYPE type); // returns the layer index
	void update(sf::Time elapsed, float nOrientation);

	inline void bindTexture() const {_texManager.bind();}

	sf::FloatRect getCurrentSprite() const;
	inline float getMaxHeightFactor() const {
		return (float) _texManager.getMaxHeight() / (float) _animInfo.at(_currentAnim).spriteSize.y;
	}

	inline sf::Vector2f getRawSize() const {return _animInfo.at(_currentAnim).spriteSize;}

	sf::Time getAnimationTime(ANM_TYPE type);

	inline void die() {_dead = true;}

private:
	int getClosestOrient(float orientation);

	ANM_TYPE _currentAnim;
	int _currentOrient;
	int _currentSprite;
	bool _dead;

	sf::Time _alreadyElapsed;
	std::map<ANM_TYPE, AnimInfo> _animInfo;

	const AnimationManagerInitializer& _texManager;
};
