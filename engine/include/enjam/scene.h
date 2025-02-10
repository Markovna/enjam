#ifndef INCLUDE_ENJAM_SCENE_H_
#define INCLUDE_ENJAM_SCENE_H_

#include <vector>

namespace Enjam {

class RenderPrimitive;

class Scene {
 public:
  using PrimitivesContainer = std::vector<RenderPrimitive>;

  PrimitivesContainer& getPrimitives() { return primitives; }
  const PrimitivesContainer& getPrimitives() const { return primitives; }

 private:
  PrimitivesContainer primitives;

};

}

#endif //INCLUDE_ENJAM_SCENE_H_
