#include <enjam/renderer_backend_vulkan.h>

namespace Enjam {

bool RendererBackendVulkan::init() {
  return false;
}

void RendererBackendVulkan::shutdown() {

}
void RendererBackendVulkan::beginFrame() {

}
void RendererBackendVulkan::endFrame() {

}
void RendererBackendVulkan::draw(ProgramHandle handle,
                                 VertexBufferHandle bufferHandle,
                                 IndexBufferHandle indexBufferHandle,
                                 uint32_t indexCount,
                                 uint32_t indexOffset) {

}
ProgramHandle RendererBackendVulkan::createProgram(ProgramData& data) {
  return Enjam::ProgramHandle();
}
void RendererBackendVulkan::destroyProgram(ProgramHandle handle) {

}
DescriptorSetHandle RendererBackendVulkan::createDescriptorSet(DescriptorSetData&& data) {
  return Enjam::DescriptorSetHandle();
}
void RendererBackendVulkan::destroyDescriptorSet(DescriptorSetHandle handle) {

}
void RendererBackendVulkan::updateDescriptorSetBuffer(DescriptorSetHandle dsh,
                                                      uint8_t binding,
                                                      BufferDataHandle bdh,
                                                      uint32_t size,
                                                      uint32_t offset) {

}
void RendererBackendVulkan::updateDescriptorSetTexture(DescriptorSetHandle dsh, uint8_t binding, TextureHandle th) {

}
void RendererBackendVulkan::bindDescriptorSet(DescriptorSetHandle dsh, uint8_t set) {

}
VertexBufferHandle RendererBackendVulkan::createVertexBuffer(std::initializer_list<VertexAttribute> list,
                                                             uint64_t vertexCount) {
  return Enjam::VertexBufferHandle();
}
void RendererBackendVulkan::assignVertexBufferData(VertexBufferHandle handle,
                                                   uint8_t attributeIndex,
                                                   BufferDataHandle dataHandle) {

}
void RendererBackendVulkan::destroyVertexBuffer(VertexBufferHandle handle) {

}
IndexBufferHandle RendererBackendVulkan::createIndexBuffer(uint32_t byteSize) {
  return Enjam::IndexBufferHandle();
}
void RendererBackendVulkan::updateIndexBuffer(IndexBufferHandle handle, BufferDataDesc&& desc, uint32_t byteOffset) {

}
void RendererBackendVulkan::destroyIndexBuffer(IndexBufferHandle handle) {

}
BufferDataHandle RendererBackendVulkan::createBufferData(uint32_t size, BufferTargetBinding binding) {
  return Enjam::BufferDataHandle();
}
void RendererBackendVulkan::updateBufferData(BufferDataHandle handle, BufferDataDesc&& desc, uint32_t byteOffset) {

}
void RendererBackendVulkan::destroyBufferData(BufferDataHandle handle) {

}
TextureHandle RendererBackendVulkan::createTexture(uint32_t width,
                                                   uint32_t height,
                                                   uint8_t levels,
                                                   TextureFormat format) {
  return Enjam::TextureHandle();
}
void RendererBackendVulkan::setTextureData(TextureHandle th,
                                           uint32_t level,
                                           uint32_t xoffset,
                                           uint32_t yoffset,
                                           uint32_t zoffset,
                                           uint32_t width,
                                           uint32_t height,
                                           uint32_t depth,
                                           const void* data) {

}
void RendererBackendVulkan::destroyTexture(TextureHandle handle) {

}

}