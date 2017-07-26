#include "log.h"

#include "camera.h"
#define LOG_REFRESH_RATE_MS 1000

void Log::addFPSandCamInfo() {
  if (_lastFPSupdate.getElapsedTime() > LOG_REFRESH_RATE_MS) {
    _lastFPS.str("");

    size_t fps = 1000 * _framesSinceLastUpdate / _lastFPSupdate.getElapsedTime();

    _lastFPS << "FPS: " << fps << std::endl;
    _framesSinceLastUpdate = 1;
    _lastFPSupdate.restart();
  }

  else
    _framesSinceLastUpdate++;

  Camera& cam = Camera::getInstance();

  #ifndef __ANDROID__
    _text << "X: " << cam.getPointedPos().x << "\n"
          << "Y: " << cam.getPointedPos().y << std::endl;
    _text << "R: " << cam.getZoom() << "\n"
          << "Theta: " << cam.getTheta() - 360 * (int) (cam.getTheta() / 360) +
                          (cam.getTheta() < 0 ? 360 : 0) << "\n"
          << "Phi: " << cam.getPhi() << std::endl;
  #endif

  _text << _lastFPS.str();
}
