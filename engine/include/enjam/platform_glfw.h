#ifndef INCLUDE_ENJAM_PLATFORM_GLFW_H_
#define INCLUDE_ENJAM_PLATFORM_GLFW_H_

#include <enjam/defines.h>

namespace Enjam {

using Platform = class PlatformGlfw;

class Engine;

class ENJAM_API PlatformGlfw {
public:
  void init(Engine& engine);
  void pollInputEvents();
  void shutdown();

private:
  void onKeyPress(int key, int scancode, int action, int mods);
  void onKeyRelease(int key, int scancode, int action, int mods);

private:
  bool initialized;
  Engine* engine;
};

}

#endif //INCLUDE_ENJAM_PLATFORM_GLFW_H_
