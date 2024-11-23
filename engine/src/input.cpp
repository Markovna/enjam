#include <enjam/input.h>
#include <enjam/log.h>
#include <enjam/engine.h>

namespace Enjam {

Input::Input(Engine& engine)
  : engine(engine) {
}

void Input::update() {
}

Input::~Input() = default;

}