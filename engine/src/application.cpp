#include <enjam/application.h>

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

}