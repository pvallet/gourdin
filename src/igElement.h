#pragma once
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <string>
#include "animationManager.h"

// ig = ingame

enum abstractType {igE, igME, CTRL};

class igElement {
public:
	igElement(sf::Vector2f position, AnimationManager _graphics);
	virtual ~igElement();
	inline virtual abstractType getAbstractType() const {return igE;}

	virtual void update(sf::Time elapsed, float theta);
	void set3DCorners(sf::Vector3f nCorners[4]);
	void set2DCorners(sf::IntRect nCorners) {corners2 = nCorners;}
	void setDepth(float nDepth) {depth = nDepth;}

	inline virtual float getMaxHeightFactor() {return graphics.getMaxHeightFactor();}
	virtual void launchAnimation (ANM_TYPE type);

	void draw() const;

	// Getters

	inline sf::Vector2f getPos() const {return pos;}
	inline float getOrientation() const {return orientation;}
	inline float getH() const {return height;}
	inline const sf::Texture* getTexture() const {return graphics.getTexture();}
	inline sf::IntRect getCurrentSprite() const {return graphics.getCurrentSprite();}
	inline sf::IntRect get2DCorners() const {return corners2;}
	inline float getDepth() const {return depth;}

protected:
	sf::Vector2f pos;
	float height;
	AnimationManager graphics;

	sf::Vector3f* corners3; // Starting with top left, then clockwise
	sf::IntRect corners2;

	float camOrientation; // Angle between the camera and the vector (0,1)

	void setOrientation(float nOrientation);

	float depth; // Distance to camera

	GLuint vbo;
	GLuint ibo;

	float* vertices;
	int* coord2D;
	GLuint* indices;

private:
	float orientation; // Angle between the front of the sprite and the camera
};

