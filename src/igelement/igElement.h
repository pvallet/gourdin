#pragma once

#include <string>

#include <SFML/Graphics.hpp>

// ig = ingame

class igElement {
public:
	igElement(sf::Vector2f position);

	virtual void updateDisplay(sf::Time elapsed, float theta);
	inline void setVertices(std::array<float,12> nVertices) {_vertices = nVertices;}
	inline void setVisible(bool nVisible) {_visible = nVisible;}

	// Getters

	virtual size_t getTexID() const = 0;

	sf::IntRect getScreenCoord() const;
	inline sf::Vector2f getPos() const {return _pos;}
	inline float getOrientation() const {return _orientation;}
	inline sf::Vector2f getSize() const {return _size;}
	inline float getDepth() const {return _vertices[2];}
	inline bool isVisible() const {return _visible;}

	inline std::array<float, 12> getVertices() const {return _vertices;}
	inline std::array<float,  8> getCoord2D()  const {return _coord2D;}
	inline std::array<float,  4> getLayer()    const {return _layer;}

protected:
	void setTexCoord(sf::FloatRect rect);
	void setLayer(size_t layer);
	void setOrientation(float nOrientation);

	sf::Vector2f _pos;
	sf::Vector2f _size;

	float _camOrientation; // Angle between the camera and the vector (0,1)

	bool _visible;

	std::array<float, 12> _vertices;
	std::array<float,  8> _coord2D;
	std::array<float,  4> _layer;

private:
	float _orientation; // Angle between the front of the sprite and the camera
};
