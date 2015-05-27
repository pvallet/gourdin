#include "animationManager.h"
#include <iostream>

AnimationManager::AnimationManager(std::string filename) :
	moving(false),
	currentAnim(0),
    currentSprite(4),
    alreadyElapsed(sf::Time::Zero) {
	sf::Image img;
	if (!img.loadFromFile(filename)) {
    	std::cout << "Unable to open file" << std::endl;
    }

    img.createMaskFromColor(sf::Color(173, 214, 214));
    tex.loadFromImage(img);
    tex.setSmooth(true);

    AnimInfo tmp;
    tmp.sprite = sf::IntRect(0, 0, 35, 64);
    tmp.steps = 12;
    tmp.duration = sf::seconds(1./12.);
    tmp.orientation = 90.;
    tmp.type = ANM_WALK;
    animInfo.push_back(tmp);

    tmp.orientation = 270.;
    animInfo.push_back(tmp);
    
    tmp.sprite = sf::IntRect(0, 0, 36, 64);
    tmp.steps = 4;
    tmp.orientation = 0.;
    tmp.type = ANM_TURN;
    animInfo.push_back(tmp);
}

void AnimationManager::launchAnimation(ANM_TYPE type, float nOrientation) {
	moving = true;

    if (animInfo[currentAnim].type != type) {
        currentAnim = getClosestAnim(type, nOrientation);
        currentSprite = 0;
    }
}

void AnimationManager::update(sf::Time elapsed, float nOrientation) {
    if (moving) {
        alreadyElapsed += elapsed;
        int add = alreadyElapsed.asMilliseconds() / animInfo[currentAnim].duration.asMilliseconds();

        if (add > 0) {
            currentSprite += add;
            currentSprite %= animInfo[currentAnim].steps;
            alreadyElapsed = sf::milliseconds(alreadyElapsed.asMilliseconds() % animInfo[currentAnim].duration.asMilliseconds());
        }
    }

    currentAnim = getClosestAnim(animInfo[currentAnim].type, nOrientation);
}

void AnimationManager::stop() {
    moving = false;
    currentSprite = 0;
    alreadyElapsed = sf::Time::Zero;
}

sf::IntRect AnimationManager::getCurrentSprite() const {
	sf::IntRect sprite = animInfo[currentAnim].sprite;

    sprite.left += currentSprite * sprite.width;
    sprite.top += currentAnim * sprite.height;

    return sprite;
}


int AnimationManager::getClosestAnim(ANM_TYPE type, float orientation) {
    float diff = 180.;
    int index = 0;

    for (unsigned int i = 0 ; i < animInfo.size(); i++) {
        if (animInfo[i].type == type) {
            float tmp = animInfo[i].orientation - orientation;

            if (tmp < 0.)
                tmp += 360.;

            if (tmp > 180.)
                tmp = 360. - tmp;

            if (tmp < diff) {
                diff = tmp;
                index = i;
            }
        }
    }

    return index;
}
