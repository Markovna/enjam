#ifndef INCLUDE_ENJAM_PLATFORM_GLFW_H_
#define INCLUDE_ENJAM_PLATFORM_GLFW_H_

#include <enjam/defines.h>
#include <enjam/renderer_backend_type.h>

class GLFWwindow;

namespace Enjam {

using Platform = class PlatformGlfw;

class Input;
class LibraryLoader;

namespace renderer {

class RendererBackend;

}

class ENJAM_API PlatformGlfw {
 public:
  explicit PlatformGlfw();
  renderer::RendererBackend* createRendererBackend(RendererBackendType = RendererBackendType::DEFAULT);
  void pollInputEvents(Input& input);
  void shutdown();

  LibraryLoader& getLibraryLoader() { return *libLoader; }

private:
  void init();

private:
  bool initialized = false;
  GLFWwindow* window;
  LibraryLoader* libLoader;
};

}

#endif //INCLUDE_ENJAM_PLATFORM_GLFW_H_
