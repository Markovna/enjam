#ifndef INCLUDE_ENJAM_RENDERVIEW_H_
#define INCLUDE_ENJAM_RENDERVIEW_H_

#include <enjam/math.h>
#include <enjam/renderer_backend.h>
#include <enjam/render_primitive.h>
#include <vector>

namespace Enjam {

class Camera {
 public:
  math::mat4f projectionMatrix;
  math::vec3f position;
  math::vec3f front;
  math::vec3f up;
};

struct PerViewUniforms {
  std140::mat44 projection;
  std140::mat44 view;
};

class RenderView {
 public:
  using PrimitivesContainer = std::vector<RenderPrimitive>;

  explicit RenderView(renderer::RendererBackend&);
  ~RenderView();

  void setCamera(Camera* cam) { camera = cam; }
  void setProgram(renderer::ProgramHandle handle) { programHandle = handle; }

  PrimitivesContainer& getPrimitives() { return primitives; }

 private:
  friend class Renderer;

  renderer::DescriptorSetHandle viewDescriptorSetHandle;
  renderer::BufferDataHandle viewUniformBufferHandle;
  PerViewUniforms perViewUniformBufferData;

  PrimitivesContainer primitives;
  Camera* camera;
  renderer::ProgramHandle programHandle;
  renderer::RendererBackend& rendererBackend;
};

}

#endif //INCLUDE_ENJAM_RENDERVIEW_H_
