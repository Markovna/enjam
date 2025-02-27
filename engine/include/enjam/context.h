#pragma once

#include "defines.h"
#include <string>
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

class App {
 public:
  App() = default;
  virtual ~App() = default;

  virtual void setup() = 0;
  virtual void tick() = 0;
  virtual void cleanup() = 0;
};

struct Config {
  std::string assetsPath;
};

class Context final {
 public:
  using CreateAppCallback = std::function<void()>;
  using TickAppCallback = std::function<void()>;
  using DestroyAppCallback = std::function<void()>;

 public:
  Context();
  ~Context();

  Context(const Context&) = delete;
  Context(Context&&) = delete;
  Context& operator=(const Context&) = delete;
  Context& operator=(Context&&) = delete;

  Platform* getPlatform() { return platform; }
  Input* getInput() { return input; }
  Renderer* getRenderer() { return renderer; }
  renderer::RendererBackend* getRendererBackend() { return rendererBackend; }
  Scene* getScene() { return scene; }
  Camera* getCamera() { return camera; }

  void onCreateApp(CreateAppCallback callback) { createAppCallback = std::move(callback); }
  void createApp() { return createAppCallback(); }

  void onDestroyApp(DestroyAppCallback callback) { destroyAppCallback = std::move(callback); }
  void destroyApp() { return destroyAppCallback(); }

  void onTickApp(TickAppCallback callback) { tickAppCallback = std::move(callback); }
  void tickApp() { return tickAppCallback(); }

 private:
  Platform* platform;
  Input* input;
  Renderer* renderer;
  renderer::RendererBackend* rendererBackend;
  App* application;
  Scene* scene;
  Camera* camera;
  CreateAppCallback createAppCallback;
  DestroyAppCallback destroyAppCallback;
  TickAppCallback tickAppCallback;
};

}