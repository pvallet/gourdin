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
	void setVertices(std::array<float,12> nVertices);
	inline void setVisible(bool nVisible) {_visible = nVisible;}

	virtual size_t draw() const;

	// Getters

	sf::IntRect getScreenCoord() const;
	inline sf::Vector2f getPos() const {return _pos;}
	inline float getOrientation() const {return _orientation;}
	inline sf::Vector2f getSize() const {return _size;}
	inline std::array<float,12> getVertices() const {return _vertices;}
	inline float getDepth() const {return _vertices[2];}
	inline bool isVisible() const {return _visible;}

protected:
	sf::Vector2f _pos;
	sf::Vector2f _size;

	float _camOrientation; // Angle between the camera and the vector (0,1)

	void setOrientation(float nOrientation);

	bool _visible;

	GLuint _vbo;
	GLuint _ibo;

	std::array<float, 12> _vertices;
	std::array<float,  8> _coord2D;
	std::array<GLuint, 4> _indices;

private:
	float _orientation; // Angle between the front of the sprite and the camera
};
