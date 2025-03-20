#ifndef INCLUDE_ENJAM_APPLICATION_H_
#define INCLUDE_ENJAM_APPLICATION_H_

#include <functional>

namespace Enjam {

class Application final {
 public:
  using SetupCallback = std::function<void()>;
  using CleanupCallback = std::function<void()>;
  using TickCallback = std::function<void()>;

  ~Application() = default;
  Application() = default;

  void run(SetupCallback, CleanupCallback, TickCallback);
  void exit() { exitRequested = true; }

 private:
  bool exitRequested = false;
};

}

#endif //INCLUDE_ENJAM_APPLICATION_H_
