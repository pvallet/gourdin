#pragma once

#include <string>

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>

// ig = ingame

class igElement {
public:
	igElement(sf::Vector2f position);
	virtual ~igElement();

	virtual void update(sf::Time elapsed, float theta);
	void set3DCorners(glm::vec3 nCorners[4]);
	void set2DCorners(const sf::IntRect& nCorners) {_corners2 = nCorners;}
	inline void setDepth(float nDepth) {_depth = nDepth;}
	inline void setVisible(bool nVisible) {_visible = nVisible;}

	virtual void draw() const;

	// Getters

	inline sf::Vector2f getPos() const {return _pos;}
	inline float getOrientation() const {return _orientation;}
	inline sf::Vector2f getSize() const {return _size;}
	inline sf::IntRect get2DCorners() const {return _corners2;}
	inline float getDepth() const {return _depth;}
	inline bool getVisible() const {return _visible;}

protected:
	sf::Vector2f _pos;
	sf::Vector2f _size;

	sf::IntRect _corners2;

	float _camOrientation; // Angle between the camera and the vector (0,1)

	void setOrientation(float nOrientation);

	float _depth; // Distance to camera
	bool _visible;

	GLuint _vbo;
	GLuint _ibo;

	std::array<float, 12> _vertices;
	std::array<float, 8>  _coord2D;
	std::array<GLuint, 4> _indices;

private:
	float _orientation; // Angle between the front of the sprite and the camera
};
