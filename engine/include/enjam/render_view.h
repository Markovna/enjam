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
  explicit RenderView(renderer::RendererBackend&);
  ~RenderView();

  void setCamera(Camera* ptr) { camera = ptr; }
  void setScene(Scene* ptr) { scene = ptr; }
  void setProgram(renderer::ProgramHandle handle) { programHandle = handle; }

 private:
  void updateViewUniformBuffer();
  void updateObjectsUniformBuffer();
  void updateObjectBuffer(uint32_t primitiveIndex);

 private:
  friend class Renderer;

  using PerObjectUniformBufferData = std::vector<PerObjectUniforms>;

  renderer::BufferDataHandle objectsUniformBufferHandle;
  PerObjectUniformBufferData perObjectUniformBufferData;

  renderer::DescriptorSetHandle viewDescriptorSetHandle;
  renderer::BufferDataHandle viewUniformBufferHandle;
  PerViewUniforms perViewUniformBufferData;

  Scene* scene;
  Camera* camera;
  renderer::ProgramHandle programHandle;
  renderer::RendererBackend& rendererBackend;
};

}

#endif //INCLUDE_ENJAM_RENDERVIEW_H_
