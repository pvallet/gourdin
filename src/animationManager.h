#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <map>


enum ANM_TYPE {WAIT, WALK, DIE, RUN, ATTACK};

typedef struct AnimInfo AnimInfo;
struct AnimInfo {
	sf::IntRect sprite;
	int steps;
	int orientations;
	sf::Time duration; 	// of each frame
	sf::Time pause; 	// Before starting the anim over
	bool loop; 			// If false, pause is infinite
};

class AnimationManager {
public:
	AnimationManager(std::vector<sf::Texture*> tex, std::string xml);

	void launchAnimation(ANM_TYPE type);
	void update(sf::Time elapsed, float nOrientation);

	inline const sf::Texture* getTexture() const {return _tex[_currentAnim];}
	sf::IntRect getCurrentSprite() const;
	inline float getMaxHeightFactor() {return (float) _maxHeight / (float) _animInfo[_currentAnim].sprite.height;}
	inline void die() {_dead = true;}

	sf::Time getAnimationTime(ANM_TYPE type);

private:
	int getClosestOrient(float orientation);

	std::vector<sf::Texture*> _tex;

	int _maxHeight;

	ANM_TYPE _currentAnim;
	int _currentOrient;
	int _currentSprite;
	bool _dead;

	sf::Time _alreadyElapsed;
	std::map<ANM_TYPE, AnimInfo> _animInfo;
};
