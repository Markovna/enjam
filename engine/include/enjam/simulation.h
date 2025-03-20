#ifndef ENJAM_ENGINE_INCLUDE_ENJAM_SIMULATION_H_
#define ENJAM_ENGINE_INCLUDE_ENJAM_SIMULATION_H_

namespace Enjam {

class Simulation {
 public:
  virtual ~Simulation() = default;
  virtual void start() = 0;
  virtual void tick() = 0;
  virtual void stop() = 0;
};

}
#endif //ENJAM_ENGINE_INCLUDE_ENJAM_SIMULATION_H_
