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
	igElement(sf::Vector2<double> position);
	virtual ~igElement();
	inline virtual abstractType getAbstractType() const {return igE;}

	virtual void update(sf::Time elapsed, float theta);
	void set3DCorners(sf::Vector3f nCorners[4]);
	void set2DCorners(sf::IntRect nCorners) {corners2 = nCorners;}
	inline void setDepth(float nDepth) {depth = nDepth;}
	inline void setVisible(bool nVisible) {visible = nVisible;}

	virtual void draw() const = 0;

	// Getters

	inline sf::Vector2<double> getPos() const {return pos;}
	inline float getOrientation() const {return orientation;}
	inline float getH() const {return height;}
	inline float getW() const {return height * (float) (coord2D[2]-coord2D[0]) / (float) (coord2D[5]-coord2D[1]);}
	inline sf::IntRect get2DCorners() const {return corners2;}
	inline float getDepth() const {return depth;}
	inline bool getVisible() const {return visible;}

protected:
	sf::Vector2<double> pos;
	float height;

	sf::Vector3f* corners3; // Starting with top left, then clockwise
	sf::IntRect corners2;

	float camOrientation; // Angle between the camera and the vector (0,1)

	void setOrientation(float nOrientation);

	float depth; // Distance to camera
	bool visible;

	GLuint vbo;
	GLuint ibo;

	float* vertices;
	int* coord2D;
	GLuint* indices;

private:
	float orientation; // Angle between the front of the sprite and the camera
};
