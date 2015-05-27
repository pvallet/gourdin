#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>


enum ANM_TYPE {ANM_WALK, ANM_TURN};

typedef struct AnimInfo AnimInfo;
struct AnimInfo {
	sf::IntRect sprite;
	int steps;
	sf::Time duration;
	float orientation;
	ANM_TYPE type;
};

class AnimationManager {
public:
	AnimationManager(std::string filename);

	void launchAnimation(ANM_TYPE type, float nOrientation);
	void update(sf::Time elapsed, float nOrientation);
	void stop();

	inline const sf::Texture* getTexture() const {return &tex;}
	sf::IntRect getCurrentSprite() const;
private:
	int getClosestAnim(ANM_TYPE type, float orientation); // Finds the best animation for the current orientation

	sf::Texture tex;

	bool moving;
	int currentAnim;
	int currentSprite;
	float nOrientation;

	sf::Time alreadyElapsed;
	std::vector<AnimInfo> animInfo;
};

