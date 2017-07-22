#include "frameBufferObject.h"

#include <SDL_log.h>

FrameBufferObject::FrameBufferObject() :
  _fboIndex(0),
  _colorBuffer(0),
  _depthBuffer(0) {}

void FrameBufferObject::init(size_t width, size_t height,
  GLenum colorBufferInternalFormat, GLenum colorBufferFormat, GLenum colorBufferType) {

  glGenTextures(1, &_colorBuffer);
  glBindTexture(GL_TEXTURE_2D, _colorBuffer);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, colorBufferInternalFormat, width, height, 0, colorBufferFormat, colorBufferType, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenTextures(1, &_depthBuffer);
  glBindTexture(GL_TEXTURE_2D, _depthBuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height,
            0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glBindTexture(GL_TEXTURE_2D, 0);

  glGenFramebuffers(1, &_fboIndex);
  glBindFramebuffer(GL_FRAMEBUFFER, _fboIndex);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorBuffer, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  GL_TEXTURE_2D, _depthBuffer, 0);

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error in FrameBufferObject::init, unable to create FBO");
    switch (glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
      case GL_FRAMEBUFFER_UNDEFINED:                     SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GL_FRAMEBUFFER_UNDEFINED"); break;
      case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:         SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"); break;
      case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"); break;
      case GL_FRAMEBUFFER_UNSUPPORTED:                   SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GL_FRAMEBUFFER_UNSUPPORTED"); break;
      case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"); break;
#ifndef __ANDROID__
      case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"); break;
      case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"); break;
      case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"); break;
#endif
    }
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
