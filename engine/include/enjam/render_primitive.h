#ifndef INCLUDE_ENJAM_RENDER_PRIMITIVE_H_
#define INCLUDE_ENJAM_RENDER_PRIMITIVE_H_

#include <enjam/math.h>
#include <enjam/renderer_backend.h>

namespace Enjam {

class IndexBuffer {
 public:
  using IndexBufferHandle = renderer::IndexBufferHandle;
  IndexBuffer(renderer::RendererBackend&, size_t indexCount);
  void setBuffer(renderer::RendererBackend&, renderer::BufferDataDesc&& desc, uint32_t byteOffset);
  void destroy(renderer::RendererBackend&);
  IndexBufferHandle& getHandle() { return handle; }

 private:
  renderer::IndexBufferHandle handle;
};

class BufferObject;

class VertexBuffer {
 public:
  using VertexBufferHandle = renderer::VertexBufferHandle;

  VertexBuffer(renderer::RendererBackend&, renderer::VertexArrayDesc&&);
  void setBuffer(renderer::RendererBackend&, renderer::BufferDataDesc&&, uint32_t offset);
  void setBuffer(renderer::RendererBackend&, BufferObject&);
  void destroy(renderer::RendererBackend&);
  VertexBufferHandle getHandle() { return handle; }

 private:
  static constexpr uint8_t FLAG_DESTROY_BUFFER_OBJECT = 0x01;

  renderer::VertexBufferHandle handle;
  renderer::BufferDataHandle bufferDataHandle;
  uint8_t flags;
};

class BufferObject {
 public:
  using BufferObjectHandle = renderer::BufferDataHandle;

  BufferObject(renderer::RendererBackend&, renderer::BufferTargetBinding, size_t);
  void setBuffer(renderer::RendererBackend&, renderer::BufferDataDesc&&, uint32_t offset);
  void destroy(renderer::RendererBackend&);

  BufferObjectHandle getHandle() { return handle; }
  renderer::BufferTargetBinding getBinding() { return binding; }

 private:
  renderer::BufferDataHandle handle;
  renderer::BufferTargetBinding binding;
};

class RenderPrimitive {
 public:
  RenderPrimitive(VertexBuffer& vertexBuffer, IndexBuffer& indexBuffer)
    : vertexBuffer(vertexBuffer), indexBuffer(indexBuffer), transform(1)
  { }

  VertexBuffer& getVertexBuffer() { return vertexBuffer; }
  IndexBuffer& getIndexBuffer() { return indexBuffer; }
  const math::mat4f& getTransform() const { return transform; }

  void setTransform(math::mat4f&& tr) { transform = std::move(tr); }

 private:
  VertexBuffer& vertexBuffer;
  IndexBuffer& indexBuffer;
  math::mat4f transform;
};

}

#endif //INCLUDE_ENJAM_RENDER_PRIMITIVE_H_
