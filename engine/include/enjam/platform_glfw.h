#ifndef INCLUDE_ENJAM_PLATFORM_GLFW_H_
#define INCLUDE_ENJAM_PLATFORM_GLFW_H_

#include <enjam/defines.h>

namespace Enjam {

using Platform = class PlatformGlfw;

typedef void *(*GLLoaderProc)(const char *name);

class Input;

class ENJAM_API PlatformGlfw {
public:
  explicit PlatformGlfw(Input& input);
  void init();
  void pollInputEvents();
  void shutdown();
  GLLoaderProc GetGLLoaderProc();

private:
  void onKeyPress(int key, int scancode, int action, int mods);
  void onKeyRelease(int key, int scancode, int action, int mods);

private:
  bool initialized = false;
  Input& input;
};

}

#endif //INCLUDE_ENJAM_PLATFORM_GLFW_H_
