#pragma once

#include <memory>

namespace Enjam {

class Input;

class Engine {
 public:
  Engine();

  void update();

  Input& getInput() { return *input; }

 private:
  std::unique_ptr<Input> input;
};

}