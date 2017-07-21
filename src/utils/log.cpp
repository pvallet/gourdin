#include "log.h"

#include "camera.h"

void Log::addFPSandCamInfo(int msElapsed) {
  Camera& cam = Camera::getInstance();
  int fps = 0;

  if (msElapsed != 0)
    fps = 1.f / msElapsed * 1000;

#ifndef __ANDROID__
  _text << "X: " << cam.getPointedPos().x << "\n"
        << "Y: " << cam.getPointedPos().y << std::endl;
  _text << "R: " << cam.getZoom() << "\n"
        << "Theta: " << cam.getTheta() - 360 * (int) (cam.getTheta() / 360) +
                        (cam.getTheta() < 0 ? 360 : 0) << "\n"
        << "Phi: " << cam.getPhi() << std::endl;
#endif

  _text << "FPS: " << fps << std::endl;
}
