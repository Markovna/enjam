#ifndef INCLUDE_ENJAM_APPLICATION_H_
#define INCLUDE_ENJAM_APPLICATION_H_

#include <functional>
#include <utility>
#include <enjam/di.h>

namespace Enjam {

class Platform;
class Renderer;
class Input;
class Scene;
class Camera;
class RenderView;
class RendererBackend;

class Simulation {
 public:
  virtual ~Simulation() = default;
  virtual void start() = 0;
  virtual void tick() = 0;
  virtual void stop() = 0;
};

class Application final {
 public:
  using SetupCallback = std::function<void()>;
  using CleanupCallback = std::function<void()>;
  using TickCallback = std::function<void()>;

  ~Application();
  Application();

  void run(SetupCallback, CleanupCallback, TickCallback);
  void exit() { exitRequested = true; }

 private:
  bool exitRequested = false;
};

}

#endif //INCLUDE_ENJAM_APPLICATION_H_
