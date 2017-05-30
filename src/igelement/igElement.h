#pragma once

#include <string>

#include <SFML/Graphics.hpp>

// ig = ingame

class igElement {
public:
	igElement(sf::Vector2f position);

	virtual void updateDisplay(sf::Time elapsed, float theta);
	inline void setHeight(float height) {_height = height; setPosArray();}

	// Getters

	virtual size_t getTexID() const = 0;

	inline sf::Vector2f getPos() const {return _pos;}
	inline float getHeight() const {return _height;}
	inline float getOrientation() const {return _orientation;}
	inline sf::Vector2f getSize() const {return _size;}

	inline std::array<float, 12> getVertices() const {return _vertices;}
	inline std::array<float, 12> getPosArray() const {return _posArray;}
	inline std::array<float,  8> getCoord2D()  const {return _coord2D;}
	inline std::array<float,  4> getLayer()    const {return _layer;}

protected:
	void setVertices(); // Uses the _size attribute
	void setPosArray();
	void setTexCoord(sf::FloatRect rect);
	void setLayer(size_t layer);
	void setOrientation(float nOrientation);

	sf::Vector2f _pos;
	sf::Vector2f _size;
	sf::Vector2f _offset; // To adapt the sprite to the terrain

	float _height;

	float _camOrientation; // Angle between the camera and the vector (0,1)

	std::array<float, 12> _vertices;
	std::array<float, 12> _posArray;
	std::array<float,  8> _coord2D;
	std::array<float,  4> _layer;

private:
	float _orientation; // Angle between the front of the sprite and the camera
};
