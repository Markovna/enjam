#ifndef INCLUDE_ENJAM_SCENE_H_
#define INCLUDE_ENJAM_SCENE_H_

#include <vector>

namespace Enjam {

class RenderPrimitive;

class Scene {
 public:
  std::vector<RenderPrimitive> primitives;
};

}

#endif //INCLUDE_ENJAM_SCENE_H_
