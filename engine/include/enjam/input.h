#pragma once

#include <enjam/defines.h>
#include <enjam/event.h>
#include <enjam/input_events.h>

namespace Enjam {

using KeyPressEvent = Event<const KeyPressEventArgs&>;
using KeyReleaseEvent = Event<const KeyReleaseEventArgs&>;
using MouseMoveEvent = Event<const MouseMoveEventArgs&>;
using MouseUpEvent = Event<const MouseUpEventArgs&>;
using MouseDownEvent = Event<const MouseDownEventArgs&>;
using MouseScrollEvent = Event<const MouseScrollEventArgs&>;

class Engine;

class ENJAM_API Input final {
 public:
  explicit Input(Engine& engine);
  ~Input();

  void update();

  KeyPressEvent& onKeyPress() { return keyPressEvent; }
  KeyReleaseEvent& onKeyRelease() { return keyReleaseEvent; }

 private:
  Engine& engine;
  KeyPressEvent keyPressEvent;
  KeyReleaseEvent keyReleaseEvent;
  MouseMoveEvent mouseMoveEvent;
  MouseUpEvent mouseUpEvent;
  MouseDownEvent mouseDownEvent;
  MouseScrollEvent mouseScrollEvent;
};

}