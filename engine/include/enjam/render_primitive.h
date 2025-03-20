#ifndef INCLUDE_ENJAM_RENDER_PRIMITIVE_H_
#define INCLUDE_ENJAM_RENDER_PRIMITIVE_H_

#include <enjam/math.h>
#include <enjam/renderer_backend.h>

namespace Enjam {

class IndexBuffer {
 public:
  IndexBuffer(RendererBackend&, size_t indexCount);
  void setBuffer(RendererBackend&, BufferDataDesc&& desc, uint32_t byteOffset);
  void destroy(RendererBackend&);
  IndexBufferHandle& getHandle() { return handle; }

 private:
  IndexBufferHandle handle;
};

class BufferObject;

class VertexBuffer {
 public:

  VertexBuffer(RendererBackend&, VertexArrayDesc&&);
  void setBuffer(RendererBackend&, BufferDataDesc&&, uint32_t offset);
  void setBuffer(RendererBackend&, BufferObject&);
  void destroy(RendererBackend&);
  VertexBufferHandle getHandle() { return handle; }

 private:
  static constexpr uint8_t FLAG_DESTROY_BUFFER_OBJECT = 0x01;

  VertexBufferHandle handle;
  BufferDataHandle bufferDataHandle;
  uint8_t flags;
};

class BufferObject {
 public:
  using BufferObjectHandle = BufferDataHandle;

  BufferObject(RendererBackend&, BufferTargetBinding, size_t);
  void setBuffer(RendererBackend&, BufferDataDesc&&, uint32_t offset);
  void destroy(RendererBackend&);

  BufferObjectHandle getHandle() { return handle; }
  BufferTargetBinding getBinding() { return binding; }

 private:
  BufferDataHandle handle;
  BufferTargetBinding binding;
};

class RenderPrimitive {
 public:
  RenderPrimitive(VertexBuffer* vertexBuffer = nullptr, IndexBuffer* indexBuffer = nullptr, ProgramHandle programHandle = { })
    : vertexBuffer(vertexBuffer), indexBuffer(indexBuffer), programHandle(programHandle), transform(1)
  { }

  VertexBuffer* getVertexBuffer() { return vertexBuffer; }
  IndexBuffer* getIndexBuffer() { return indexBuffer; }

  void setVertexBuffer(VertexBuffer* buffer) { vertexBuffer = buffer; }
  void setIndexBuffer(IndexBuffer* buffer) { indexBuffer = buffer; }

  ProgramHandle getProgramHandle() { return programHandle; }
  void setProgramHandle(ProgramHandle handle) { programHandle = handle; }

  DescriptorSetHandle getDescriptorSetHandle() { return descriptorSetHandle; }
  void setDescriptorSetHandle(DescriptorSetHandle handle) { descriptorSetHandle = handle; }

  const math::mat4f& getTransform() const { return transform; }
  void setTransform(math::mat4f&& tr) { transform = tr; }

 private:
  VertexBuffer* vertexBuffer;
  IndexBuffer* indexBuffer;
  ProgramHandle programHandle;
  DescriptorSetHandle descriptorSetHandle;
  math::mat4f transform;
};

}

#endif //INCLUDE_ENJAM_RENDER_PRIMITIVE_H_
