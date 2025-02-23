#include <enjam/context.h>
#include <enjam/log.h>
#include <enjam/assert.h>
#include <enjam/input.h>
#include <enjam/library_loader.h>
#include <enjam/platform.h>
#include <enjam/renderer.h>
#include <enjam/render_view.h>
#include <enjam/scene.h>
#include <memory>
#include <fstream>
#include <sstream>
#include "enjam/platform_glfw.h"

namespace Enjam {

Context& Context::get() {
  static Context context;
  return context;
}

void Context::init() {
  platform = new PlatformGlfw;
  rendererBackend = platform->createRendererBackend();
  renderer = new Renderer(*rendererBackend);
  input = new Input;
  scene = new Scene;
  camera = new Camera;
}

void Context::destroy() {
  delete camera;
  delete scene;
  delete input;
  delete renderer;
  delete rendererBackend;
  delete platform;
}

}
