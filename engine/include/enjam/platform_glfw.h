#ifndef INCLUDE_ENJAM_PLATFORM_GLFW_H_
#define INCLUDE_ENJAM_PLATFORM_GLFW_H_

#include <enjam/defines.h>
#include <enjam/renderer_backend_type.h>

class GLFWwindow;

namespace Enjam {

using Platform = class PlatformGlfw;

class Input;

namespace renderer {

class RendererBackend;

}

class ENJAM_API PlatformGlfw {
 public:
  explicit PlatformGlfw(Input& input);
  renderer::RendererBackend* createRendererBackend(RendererBackendType = RendererBackendType::DEFAULT);
  void pollInputEvents();
  void shutdown();

private:
  void init();
  void onKeyPress(int key, int scancode, int action, int mods);
  void onKeyRelease(int key, int scancode, int action, int mods);

private:
  bool initialized = false;
  Input& input;
  GLFWwindow* window;
};

}

#endif //INCLUDE_ENJAM_PLATFORM_GLFW_H_
