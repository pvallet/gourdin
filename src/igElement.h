#pragma once

#include <string>

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>

// ig = ingame

enum abstractType {igE, igME, CTRL};

class igElement {
public:
	igElement(sf::Vector2<double> position);
	virtual ~igElement();
	inline virtual abstractType getAbstractType() const {return igE;}

	virtual void update(sf::Time elapsed, float theta);
	void set3DCorners(glm::vec3 nCorners[4]);
	void set2DCorners(sf::IntRect nCorners) {_corners2 = nCorners;}
	inline void setDepth(float nDepth) {_depth = nDepth;}
	inline void setVisible(bool nVisible) {_visible = nVisible;}

	virtual void draw() const = 0;

	// Getters

	inline sf::Vector2<double> getPos() const {return _pos;}
	inline float getOrientation() const {return _orientation;}
	inline float getH() const {return _height;}
	inline float getW() const {return _height * (float) (_coord2D[2]-_coord2D[0]) / (float) (_coord2D[5]-_coord2D[1]);}
	inline sf::IntRect get2DCorners() const {return _corners2;}
	inline float getDepth() const {return _depth;}
	inline bool getVisible() const {return _visible;}

protected:
	sf::Vector2<double> _pos;
	float _height;

	sf::IntRect _corners2;

	float _camOrientation; // Angle between the camera and the vector (0,1)

	void setOrientation(float nOrientation);

	float _depth; // Distance to camera
	bool _visible;

	GLuint _vbo;
	GLuint _ibo;

	float* _vertices;
	int* _coord2D;
	GLuint* _indices;

private:
	float _orientation; // Angle between the front of the sprite and the camera
};
