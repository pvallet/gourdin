#pragma once

class GLObjectInterface {
public:
  GLObjectInterface() {}
  GLObjectInterface (GLObjectInterface const&) = delete;
  GLObjectInterface& operator=(GLObjectInterface const&) = delete;
  GLObjectInterface& operator=(GLObjectInterface&&) = delete;

  virtual void bind() const = 0;
  // Derived classes should declare the method: static void unbind()
};
