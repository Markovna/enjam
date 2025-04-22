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

VertexBuffer::VertexBuffer(RendererBackend& backend, std::initializer_list<VertexAttribute> attributes, uint64_t count) {
  handle = backend.createVertexBuffer(attributes, count);
}

void VertexBuffer::setBuffer(RendererBackend& backend, uint32_t attributeIndex, BufferObject& bufferObj) {
  auto& bufferHandle = bufferHandles[attributeIndex];
  if(bufferHandle && bufferHandle.value()) {
    backend.destroyBufferData(bufferHandle.value());
    bufferHandle.reset();
  }

  backend.assignVertexBufferData(handle, attributeIndex, bufferObj.getHandle());
}

void VertexBuffer::setBuffer(RendererBackend& backend, uint32_t attributeIndex, BufferDataDesc&& desc, uint32_t offset) {
  auto& bufferHandle = bufferHandles[attributeIndex];
  if(bufferHandle && bufferHandle.value()) {
    backend.destroyBufferData(bufferHandle.value());
    bufferHandle.reset();
  }

  auto newBufferHandle = backend.createBufferData(desc.size, BufferTargetBinding::VERTEX);
  backend.updateBufferData(newBufferHandle, std::move(desc), offset);
  backend.assignVertexBufferData(handle, attributeIndex, newBufferHandle);

  bufferHandle = newBufferHandle;
}

void VertexBuffer::destroy(RendererBackend& backend) {
  if(handle) {
    backend.destroyVertexBuffer(handle);
  }

  for(auto& bufferHandle : bufferHandles) {
    if(bufferHandle && bufferHandle.value()) {
      backend.destroyBufferData(bufferHandle.value());
      bufferHandle.reset();
    }
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