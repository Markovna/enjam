#ifndef INCLUDE_ENJAM_APPLICATION_H_
#define INCLUDE_ENJAM_APPLICATION_H_

#include <functional>
#include <utility>

namespace Enjam {

class Platform;
class Renderer;
class Input;
class Scene;
class Camera;
class RenderView;

namespace renderer {
class RendererBackend;
}

class Simulation {
 public:
  virtual ~Simulation() = default;
  virtual void start() = 0;
  virtual void tick() = 0;
  virtual void stop() = 0;
};

class Application final {
 public:
  using SimulationFactory = std::function<std::unique_ptr<Simulation>()>;
  using SetupCallback = std::function<void()>;
  using CleanupCallback = std::function<void()>;
  using TickCallback = std::function<void()>;

  static Application& get() {
    static Application instance;
    return instance;
  }

  ~Application();

  void run(SetupCallback, CleanupCallback, TickCallback);
  void exit() { exitRequested = true; }

  void setSimulationFactory(SimulationFactory simulationFactory) { mSimulationFactory = std::move(simulationFactory); }
  std::unique_ptr<Simulation> createSimulation() { return mSimulationFactory(); }

  Platform* getPlatform() { return platform; }
  Input* getInput() { return input; }
  Renderer* getRenderer() { return renderer; }
  renderer::RendererBackend* getRendererBackend() { return rendererBackend; }
  Scene* getScene() { return scene; }
  Camera* getCamera() { return camera; }

 private:
  Application();

 private:
  SimulationFactory mSimulationFactory = nullptr;
  Platform* platform = nullptr;
  Input* input = nullptr;
  Renderer* renderer = nullptr;
  renderer::RendererBackend* rendererBackend = nullptr;
  Scene* scene = nullptr;
  Camera* camera = nullptr;

  bool exitRequested = false;
};

}

#endif //INCLUDE_ENJAM_APPLICATION_H_
