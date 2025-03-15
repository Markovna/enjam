#include <enjam/application.h>
#include <enjam/input.h>
#include <enjam/platform_glfw.h>
#include <enjam/renderer.h>
#include <enjam/render_view.h>
#include <enjam/scene.h>

namespace Enjam {

void Application::run(SetupCallback setup, CleanupCallback cleanup, TickCallback tick) {
  setup();

  while(!exitRequested) {
    if(tick) {
      tick();
    }
  }

  cleanup();
}

Application::Application() {

}

Application::~Application() {
}

}