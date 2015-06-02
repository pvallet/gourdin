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
	AnimationManager(std::vector<sf::Texture*> _tex, std::string xml);

	void launchAnimation(ANM_TYPE type);
	void update(sf::Time elapsed, float nOrientation);

	inline const sf::Texture* getTexture() const {return tex[currentAnim];}
	sf::IntRect getCurrentSprite() const;
	inline float getMaxHeightFactor() {return (float) maxHeight / (float) animInfo[currentAnim].sprite.height;}
	inline void die() {dead = true;}

	sf::Time getAnimationTime(ANM_TYPE type);

private:
	int getClosestOrient(float orientation);

	std::vector<sf::Texture*> tex;

	int maxHeight;

	ANM_TYPE currentAnim;
	int currentOrient;
	int currentSprite;
	float nOrientation;
	bool dead;

	sf::Time alreadyElapsed;
	std::map<ANM_TYPE, AnimInfo> animInfo;
};

