#ifndef INCLUDE_ENJAM_PLATFORM_GLFW_H_
#define INCLUDE_ENJAM_PLATFORM_GLFW_H_

#include <enjam/platform.h>
#include <memory>

class GLFWwindow;

namespace Enjam {

class ENJAM_API PlatformGlfw : public Platform {
 public:
  PlatformGlfw();
  std::unique_ptr<renderer::RendererBackend> createRendererBackend(RendererBackendType = RendererBackendType::DEFAULT) override;
  void pollInputEvents(Input& input) override;
  void shutdown();

private:
  void init();

private:
  bool initialized = false;
  GLFWwindow* window;
};

}

#endif //INCLUDE_ENJAM_PLATFORM_GLFW_H_
