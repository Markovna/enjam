#ifndef INCLUDE_ENJAM_RENDERVIEW_H_
#define INCLUDE_ENJAM_RENDERVIEW_H_

#include <enjam/scene.h>
#include <enjam/math.h>

namespace Enjam {

class Camera {
 public:
  math::mat4f projectionMatrix;
  math::vec3f position;
  math::vec3f front;
  math::vec3f up;
};

class RenderView {
 public:
  Camera camera;
  Scene scene;
};

}

#endif //INCLUDE_ENJAM_RENDERVIEW_H_
