#include <enjam/context.h>
#include <enjam/input.h>
#include <enjam/renderer.h>
#include <enjam/render_view.h>
#include <enjam/scene.h>
#include <enjam/platform_glfw.h>

namespace Enjam {

Context::Context() {
  platform = new PlatformGlfw;
  rendererBackend = platform->createRendererBackend();
  renderer = new Renderer(*rendererBackend);
  input = new Input;
  scene = new Scene;
  camera = new Camera;
}

Context::~Context() {
  delete camera;
  delete scene;
  delete input;
  delete renderer;
  delete rendererBackend;
  delete platform;
}

}
