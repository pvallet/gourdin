#pragma once

#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "utils.h"

#include <cmath>

#define ELEMENT_NEAR_PLANE 20.f

// r, theta, phi follows the mathematical convention: theta is the azimuthal and phi the polar angle

#define MIN_R 120.f
#define INIT_R 150.f
#define INIT_PHI 60.f
#define INIT_THETA 180.f

class Controller;
class EventHandler;
class EventHandlerGame;
class EventHandlerSandbox;
class Game;

class Camera {
public:
	static Camera& getInstance() {
    static Camera instance;
    return instance;
  }

	Camera(Camera const&)          = delete;
	void operator=(Camera const&)  = delete;

	inline unsigned int getW() const {return _W;}
	inline unsigned int getH() const {return _H;}
  inline sf::Vector3f getPos() const {return _pos;}
  inline float getTheta() const {return _theta;}
  inline float getPhi() const {return _phi;}
  inline float getFov() const {return _fovAngle;}
  inline float getRatio() const {return _aspectRatio;}
  inline sf::Vector2f getPointedPos() const {return sf::Vector2f(_x, _y);}
	inline float getHeight() const {return _height;}
  inline glm::mat4 getViewProjectionMatrix() const {return _viewProjection;}
	inline glm::mat4 getSkyboxViewProjectionMatrix() const {return _skyboxViewProjection;}
	inline float getZoom() const {return _r;}

	// To resize the camera
	friend Controller;
	friend EventHandler;

	friend EventHandlerGame;
	friend EventHandlerSandbox;
	friend Game;

private:
	Camera();

	void resize(unsigned int W, unsigned int H);
	void apply();

	void translate (float dWinX, float dWinY);
	void rotate (float dtheta, float dphi);
	inline void zoom (float dr) {_r += dr; _r = _r < MIN_R ? MIN_R : _r;}
	inline void setTheta(float theta) {_theta = theta;}
	inline void setPhi  (float phi) {_phi = phi;}
	inline void setZoom (float r) {_r = r;}
	inline void setValues (float r, float theta, float phi) {_r = r; _theta = theta; _phi = phi;}

  inline void setPointedPos(sf::Vector2f newPos) {_x = newPos.x; _y = newPos.y;}
  inline void setHeight(float nHeight) {_height = nHeight + _additionalHeight;}
	inline void setAdditionalHeight(float nAddHeight) {_additionalHeight = nAddHeight;}

	unsigned int _W, _H;

  float _fovAngle;
  float _aspectRatio;
  float _nearPlane;
  float _farPlane;

	float _additionalHeight;
  float _x, _y, _height;
  float _r, _phi, _theta; // The camera points towards the center of the sphere
  sf::Vector3f _pos;

  glm::mat4 _projection;
  glm::mat4 _view;
  glm::mat4 _viewProjection;
	glm::mat4 _skyboxView;
	glm::mat4 _skyboxViewProjection;
};
