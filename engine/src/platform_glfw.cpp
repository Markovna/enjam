#include <enjam/platform_glfw.h>
#include <enjam/engine.h>
#include <enjam/input_events.h>
#include <enjam/input.h>
#include <enjam/assert.h>
#include <GLFW/glfw3.h>

namespace Enjam {

void PlatformGlfw::init(Engine& engine) {
  ENJAM_ASSERT(!initialized);

  this->engine = &engine;

  int status = glfwInit();
  ENJAM_ASSERT(status == GLFW_TRUE);

  initialized = true;

  glfwSetErrorCallback([](int error_code, const char* description) {
    ENJAM_ERROR("GLFW error: {} (Code: {})", description, error_code);
  });

  GLFWwindow* win = glfwCreateWindow(800, 640, "Enjam", NULL, NULL);
  ENJAM_ASSERT(win != nullptr);

  glfwSetWindowUserPointer(win, this);

  glfwMakeContextCurrent(win);

  // set input callbacks
  glfwSetKeyCallback(win, [](GLFWwindow *w, int key, int scancode, int action, int mods) {
    auto* platform = (PlatformGlfw*) glfwGetWindowUserPointer(w);
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
      platform->onKeyPress(key, scancode, action, mods);
    }
    else if (action == GLFW_RELEASE) {
      platform->onKeyRelease(key, scancode, action, mods);
    }
  });
}

void PlatformGlfw::pollInputEvents() {
  glfwPollEvents();
}

void PlatformGlfw::onKeyRelease(int key, int scancode, int action, int mods) {
  engine->getInput().onKeyRelease().call(
      KeyReleaseEventArgs {
          .keyCode = KeyCode(key),
          .control = bool(mods & GLFW_MOD_CONTROL),
          .shift = bool(mods & GLFW_MOD_SHIFT),
          .alt = bool(mods & GLFW_MOD_ALT),
          .super = bool(mods & GLFW_MOD_SUPER)
      });
}

void PlatformGlfw::onKeyPress(int key, int scancode, int action, int mods) {
  engine->getInput().onKeyPress().call(
      KeyPressEventArgs {
          .keyCode = KeyCode(key),
          .control = bool(mods & GLFW_MOD_CONTROL),
          .shift = bool(mods & GLFW_MOD_SHIFT),
          .alt = bool(mods & GLFW_MOD_ALT),
          .super = bool(mods & GLFW_MOD_SUPER),
          .repeat = action == GLFW_REPEAT
      });
}

void PlatformGlfw::shutdown() {

}

}