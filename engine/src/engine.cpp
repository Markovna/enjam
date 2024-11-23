#include <enjam/engine.h>
#include <enjam/input.h>

namespace Enjam {

Engine::Engine() {
  input = std::make_unique<Input>(*this);
}

void Engine::update() {
  input->update();
}

}