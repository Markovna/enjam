#include <enjam/render_primitive.h>

namespace Enjam {

IndexBuffer::IndexBuffer(RendererBackend& backend, size_t indexCount) {
  uint8_t constexpr indexByteSize = sizeof(uint32_t);
  handle = backend.createIndexBuffer(indexCount * indexByteSize);
}

void IndexBuffer::setBuffer(RendererBackend& backend, BufferDataDesc&& desc, uint32_t byteOffset) {
  backend.updateIndexBuffer(handle, std::move(desc), byteOffset);
}

void IndexBuffer::destroy(RendererBackend& backend) {
  backend.destroyIndexBuffer(handle);
}

VertexBuffer::VertexBuffer(RendererBackend& backend, VertexArrayDesc&& desc) {
  handle = backend.createVertexBuffer(std::move(desc));
}

void VertexBuffer::setBuffer(RendererBackend& backend, BufferObject& buffer) {
  if(bufferDataHandle && (flags & VertexBuffer::FLAG_DESTROY_BUFFER_OBJECT)) {
    backend.destroyBufferData(bufferDataHandle);
  }

  bufferDataHandle = buffer.getHandle();
  backend.assignVertexBufferData(handle, bufferDataHandle);
  flags &= ~VertexBuffer::FLAG_DESTROY_BUFFER_OBJECT;
}

void VertexBuffer::setBuffer(RendererBackend& backend, BufferDataDesc&& desc, uint32_t offset) {
  if(bufferDataHandle && (flags & VertexBuffer::FLAG_DESTROY_BUFFER_OBJECT)) {
    backend.destroyBufferData(bufferDataHandle);
  }

  bufferDataHandle = backend.createBufferData(desc.size, BufferTargetBinding::VERTEX);
  backend.updateBufferData(bufferDataHandle, std::move(desc), offset);
  backend.assignVertexBufferData(handle, bufferDataHandle);
  flags |= VertexBuffer::FLAG_DESTROY_BUFFER_OBJECT;
}

void VertexBuffer::destroy(RendererBackend& backend) {
  if(handle) {
    backend.destroyVertexBuffer(handle);
  }

  if(bufferDataHandle && (flags & VertexBuffer::FLAG_DESTROY_BUFFER_OBJECT)) {
    backend.destroyBufferData(bufferDataHandle);
  }
}

BufferObject::BufferObject(RendererBackend& backend, BufferTargetBinding binding, size_t size) {
  handle = backend.createBufferData(size, binding);
}

void BufferObject::setBuffer(RendererBackend& backend, BufferDataDesc&& desc, uint32_t offset) {
  backend.updateBufferData(handle, std::move(desc), offset);
}

void BufferObject::destroy(RendererBackend& backend) {
  backend.destroyBufferData(handle);
}

}