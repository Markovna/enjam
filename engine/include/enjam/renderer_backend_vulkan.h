#ifndef ENGINE_INCLUDE_ENJAM_RENDERER_BACKEND_VULKAN_H_
#define ENGINE_INCLUDE_ENJAM_RENDERER_BACKEND_VULKAN_H_

#include <enjam/renderer_backend.h>
#include <vulkan/vulkan.h>

namespace Enjam {

class RendererBackendVulkan : public RendererBackend {
 public:
  bool init() override;
  void shutdown() override;
  void beginFrame() override;
  void endFrame() override;
  void draw(ProgramHandle handle,
            VertexBufferHandle bufferHandle,
            IndexBufferHandle indexBufferHandle,
            uint32_t indexCount,
            uint32_t indexOffset) override;
  ProgramHandle createProgram(ProgramData& data) override;
  void destroyProgram(ProgramHandle handle) override;
  DescriptorSetHandle createDescriptorSet(DescriptorSetData&& data) override;
  void destroyDescriptorSet(DescriptorSetHandle handle) override;
  void updateDescriptorSetBuffer(DescriptorSetHandle dsh,
                                 uint8_t binding,
                                 BufferDataHandle bdh,
                                 uint32_t size,
                                 uint32_t offset) override;
  void updateDescriptorSetTexture(DescriptorSetHandle dsh, uint8_t binding, TextureHandle th) override;
  void bindDescriptorSet(DescriptorSetHandle dsh, uint8_t set) override;
  VertexBufferHandle createVertexBuffer(std::initializer_list<VertexAttribute> list, uint64_t vertexCount) override;
  void assignVertexBufferData(VertexBufferHandle handle, uint8_t attributeIndex, BufferDataHandle dataHandle) override;
  void destroyVertexBuffer(VertexBufferHandle handle) override;
  IndexBufferHandle createIndexBuffer(uint32_t byteSize) override;
  void updateIndexBuffer(IndexBufferHandle handle, BufferDataDesc&& desc, uint32_t byteOffset) override;
  void destroyIndexBuffer(IndexBufferHandle handle) override;
  BufferDataHandle createBufferData(uint32_t size, BufferTargetBinding binding) override;
  void updateBufferData(BufferDataHandle handle, BufferDataDesc&& desc, uint32_t byteOffset) override;
  void destroyBufferData(BufferDataHandle handle) override;
  TextureHandle createTexture(uint32_t width, uint32_t height, uint8_t levels, TextureFormat format) override;
  void setTextureData(TextureHandle th,
                      uint32_t level,
                      uint32_t xoffset,
                      uint32_t yoffset,
                      uint32_t zoffset,
                      uint32_t width,
                      uint32_t height,
                      uint32_t depth,
                      const void* data) override;
  void destroyTexture(TextureHandle handle) override;
};

}

#endif //ENGINE_INCLUDE_ENJAM_RENDERER_BACKEND_VULKAN_H_
