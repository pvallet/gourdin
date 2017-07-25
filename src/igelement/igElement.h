#pragma once

#include <glm/glm.hpp>

#include "texArray.h"

#include <array>
#include <string>

// ig = ingame

class igElement {
public:
	igElement(glm::vec2 position);

	virtual void updateDisplay(int msElapsed, float theta);
	inline void setHeight(float height) {_height = height; setPosArray();}

	// Getters

	virtual const TextureArray* getTexArray() const = 0;

	inline glm::vec2 getPos() const {return _pos;}
	inline float getHeight() const {return _height;}
	inline float getOrientation() const {return _orientation;}
	inline glm::vec2 getSize() const {return _size;}

	inline std::array<float, 12> getVertices() const {return _vertices;}
	inline std::array<float, 12> getPosArray() const {return _posArray;}
	inline std::array<float,  8> getCoord2D()  const {return _coord2D;}
	inline std::array<float,  4> getLayer()    const {return _layer;}

protected:
	void setVertices(); // Uses the _size attribute
	void setPosArray();
	void setTexCoord(glm::vec4 rect);
	void setLayer(size_t layer);
	void setOrientation(float nOrientation);

	glm::vec2 _pos;
	glm::vec2 _size;
	glm::vec2 _offset; // To adapt the sprite to the terrain

	float _height;

	float _camOrientation; // Angle between the camera and the vector (0,1)

	std::array<float, 12> _vertices;
	std::array<float, 12> _posArray;
	std::array<float,  8> _coord2D;
	std::array<float,  4> _layer;

private:
	float _orientation; // Angle between the front of the sprite and the camera
};
