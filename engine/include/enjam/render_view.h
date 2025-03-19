#ifndef INCLUDE_ENJAM_RENDERVIEW_H_
#define INCLUDE_ENJAM_RENDERVIEW_H_

#include <enjam/math.h>
#include <enjam/renderer_backend.h>
#include <enjam/render_primitive.h>
#include <vector>

namespace Enjam {

struct Camera {
  math::mat4f projectionMatrix;
  math::mat4f viewMatrix;
  math::vec3f position;
  math::vec3f front;
  math::vec3f up;
};

struct PerViewUniforms {
  std140::mat44 projection;
  std140::mat44 view;
};

struct PerObjectUniforms {
  std140::mat44 model;
};

class Scene;

class RenderView {
 public:
  ~RenderView() = default;

  void setCamera(Camera* ptr) { camera = ptr; }
  void setScene(Scene* ptr) { scene = ptr; }

 private:
  friend class Renderer;

  void prepareBuffers();
  void updateViewUniformBuffer(RendererBackend& backend, BufferDataHandle);
  void updateObjectUniformBuffer(RendererBackend& backend, BufferDataHandle, uint32_t primitiveIndex);

 private:
  using PerObjectUniformBufferData = std::vector<PerObjectUniforms>;

  PerObjectUniformBufferData perObjectUniformBufferData;
  PerViewUniforms perViewUniformBufferData;

  Scene* scene;
  Camera* camera;
};

}

#endif //INCLUDE_ENJAM_RENDERVIEW_H_
