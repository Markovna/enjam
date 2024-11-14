#include <enjam/window.h>

namespace Enjam {

Window::Window() = default;
Window::~Window() = default;

void Window::update() {
  keyPressEvent.call(*this, KeyPressEventData { .keyCode = KeyCode::A });
}

}