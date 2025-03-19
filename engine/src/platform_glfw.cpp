#include <enjam/platform_glfw.h>
#include <enjam/input_events.h>
#include <enjam/input.h>
#include <enjam/assert.h>
#include <enjam/renderer_backend_opengl.h>
#include <enjam/library_loader.h>
#include <GLFW/glfw3.h>

namespace Enjam {

PlatformGlfw::PlatformGlfw()
  : initialized(false) {
  init();
}

std::unique_ptr<RendererBackend> PlatformGlfw::createRendererBackend(RendererBackendType type) {
  ENJAM_ASSERT(window);

  switch (type) {
    case DEFAULT:
    case OPENGL: {
      GLSwapChain swapChain {
          .makeCurrent = [win = window]() { glfwMakeContextCurrent(win); },
          .swapBuffers = [win = window]() { glfwSwapBuffers(win); }
      };
      return std::make_unique<RendererBackendOpengl>((GLLoaderProc) glfwGetProcAddress, swapChain);
    }
    case VULKAN:
      ENJAM_ERROR("VULKAN renderer backend is not supported for current platform.");
      return { };
    case DIRECTX:
      ENJAM_ERROR("DIRECTX renderer backend is not supported for current platform.");
      return { };
  }
}

void PlatformGlfw::init() {
  ENJAM_ASSERT(!initialized);

  int status = glfwInit();
  ENJAM_ASSERT(status == GLFW_TRUE);

  initialized = true;

  glfwSetErrorCallback([](int error_code, const char* description) {
    ENJAM_ERROR("GLFW error: {} (Code: {})", description, error_code);
  });

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  window = glfwCreateWindow(800, 640, "Enjam", NULL, NULL);
  ENJAM_ASSERT(window != nullptr);

  glfwMakeContextCurrent(window);

  // set input callbacks
  glfwSetKeyCallback(window, [](GLFWwindow *w, int key, int scancode, int action, int mods) {
    auto input = (Input*) glfwGetWindowUserPointer(w);
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
      input->onKeyPress().call(
          KeyPressEventArgs {
              .keyCode = KeyCode(key),
              .control = bool(mods & GLFW_MOD_CONTROL),
              .shift = bool(mods & GLFW_MOD_SHIFT),
              .alt = bool(mods & GLFW_MOD_ALT),
              .super = bool(mods & GLFW_MOD_SUPER),
              .repeat = action == GLFW_REPEAT
          });
    }
    else if (action == GLFW_RELEASE) {
      input->onKeyRelease().call(
          KeyReleaseEventArgs {
              .keyCode = KeyCode(key),
              .control = bool(mods & GLFW_MOD_CONTROL),
              .shift = bool(mods & GLFW_MOD_SHIFT),
              .alt = bool(mods & GLFW_MOD_ALT),
              .super = bool(mods & GLFW_MOD_SUPER)
          });
    }
  });
}

void PlatformGlfw::pollInputEvents(Input& input) {
  glfwSetWindowUserPointer(window, &input);
  glfwPollEvents();
  glfwSetWindowUserPointer(window, nullptr);
}

void PlatformGlfw::shutdown() {
  glfwTerminate();
}

}