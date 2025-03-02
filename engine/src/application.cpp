#include <enjam/application.h>
#include <enjam/input.h>
#include <enjam/platform_glfw.h>
#include <enjam/renderer.h>
#include <enjam/render_view.h>
#include <enjam/scene.h>

namespace Enjam {

void Application::run(SetupCallback setup, CleanupCallback cleanup, TickCallback tick) {
  renderer->init();

  setup();

  while(!exitRequested) {
    if(tick) {
      tick();
    }
  }

  cleanup();

  renderer->shutdown();
}

Application::Application() {
  platform = new PlatformGlfw;
  rendererBackend = platform->createRendererBackend();
  renderer = new Renderer(*rendererBackend);
  input = new Input;
  scene = new Scene;
  camera = new Camera;
}

Application::~Application() {
  ENJAM_INFO("Destroying application");
  delete camera;
  delete scene;
  delete input;
  delete renderer;
  delete rendererBackend;
  delete platform;
  ENJAM_INFO("Application destroyed!");
}

}