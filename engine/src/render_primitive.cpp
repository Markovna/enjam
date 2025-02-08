#include <enjam/render_primitive.h>

namespace Enjam {

IndexBuffer::IndexBuffer(renderer::RendererBackend& backend, size_t indexCount) {
  uint8_t constexpr indexByteSize = 4;
  handle = backend.createIndexBuffer(indexCount * indexByteSize);
}

void IndexBuffer::setBuffer(renderer::RendererBackend& backend, renderer::BufferDataDesc&& desc, uint32_t byteOffset) {
  backend.updateIndexBuffer(handle, std::move(desc), byteOffset);
}

void IndexBuffer::destroy(renderer::RendererBackend& backend) {
  backend.destroyIndexBuffer(handle);
}

VertexBuffer::VertexBuffer(renderer::RendererBackend& backend, renderer::VertexArrayDesc&& desc) {
  handle = backend.createVertexBuffer(std::move(desc));
}

void VertexBuffer::setBuffer(renderer::RendererBackend& backend, BufferObject& buffer) {
  if(bufferDataHandle && (flags & VertexBuffer::FLAG_DESTROY_BUFFER_OBJECT)) {
    backend.destroyBufferData(bufferDataHandle);
  }

  bufferDataHandle = buffer.getHandle();
  backend.assignVertexBufferData(handle, bufferDataHandle);
  flags &= ~VertexBuffer::FLAG_DESTROY_BUFFER_OBJECT;
}

void VertexBuffer::setBuffer(renderer::RendererBackend& backend, renderer::BufferDataDesc&& desc, uint32_t offset) {
  if(bufferDataHandle && (flags & VertexBuffer::FLAG_DESTROY_BUFFER_OBJECT)) {
    backend.destroyBufferData(bufferDataHandle);
  }

  bufferDataHandle = backend.createBufferData(desc.size, renderer::BufferTargetBinding::VERTEX);
  backend.updateBufferData(bufferDataHandle, std::move(desc), offset);
  backend.assignVertexBufferData(handle, bufferDataHandle);
  flags |= VertexBuffer::FLAG_DESTROY_BUFFER_OBJECT;
}

void VertexBuffer::destroy(renderer::RendererBackend& backend) {
  if(handle) {
    backend.destroyVertexBuffer(handle);
  }

  if(bufferDataHandle && (flags & VertexBuffer::FLAG_DESTROY_BUFFER_OBJECT)) {
    backend.destroyBufferData(bufferDataHandle);
  }
}

BufferObject::BufferObject(renderer::RendererBackend& backend, renderer::BufferTargetBinding binding, size_t size) {
  handle = backend.createBufferData(size, binding);
}

void BufferObject::setBuffer(renderer::RendererBackend& backend, renderer::BufferDataDesc&& desc, uint32_t offset) {
  backend.updateBufferData(handle, std::move(desc), offset);
}

void BufferObject::destroy(renderer::RendererBackend& backend) {
  backend.destroyBufferData(handle);
}

}