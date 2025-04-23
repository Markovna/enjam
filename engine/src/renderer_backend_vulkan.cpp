#include <enjam/renderer_backend_vulkan.h>
#include <enjam/vulkan_defines.h>
#include <enjam/log.h>
#include <set>
#include <vulkan/vulkan.h>

namespace Enjam {

#if ENJAM_VULKAN_ENABLED(ENJAM_VULKAN_DEBUG_UTILS)
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

  if(messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    ENJAM_ERROR(pCallbackData->pMessage);
  }
  else {
    ENJAM_WARN(pCallbackData->pMessage);
  }

  return VK_FALSE;
}
#endif

bool RendererBackendVulkan::init() {
#if ENJAM_VULKAN_ENABLED(ENJAM_VULKAN_DEBUG_UTILS)
  VkDebugUtilsMessengerCreateInfoEXT createInfo {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = debugCallback,
      .pUserData = nullptr
  };
  VkResult result = vkCreateDebugUtilsMessengerEXT(instance, &createInfo, vkAlloc, &debugMessenger);
  if(result != VK_SUCCESS) {
    ENJAM_ERROR("Failed to set up vulkan debug messenger!");
  }
#endif


  return false;
}

void RendererBackendVulkan::shutdown() {
  vkDestroyInstance(instance, nullptr);

#if ENJAM_VULKAN_ENABLED(ENJAM_VULKAN_DEBUG_UTILS)
  if (debugMessenger) {
    vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
  }
#endif
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