#include <enjam/renderer_backend_vulkan.h>
#include <unordered_set>
#include <enjam/vulkan_defines.h>
#include <enjam/log.h>
#include <set>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_beta.h>
#include <enjam/vulkan_utils.h>

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

VkResult vkCreateDebugUtilsMessenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  }
  return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void vkDestroyDebugUtilsMessenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}

void createDebugUtilsMessenger(VkInstance instance, VkAllocationCallbacks* alloc, VkDebugUtilsMessengerEXT* messenger) {
  VkDebugUtilsMessengerCreateInfoEXT createInfo {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = debugCallback,
      .pUserData = nullptr
  };
  VkResult result = vkCreateDebugUtilsMessenger(instance, &createInfo, alloc, messenger);
  if(result != VK_SUCCESS) {
    ENJAM_ERROR("Failed to set up vulkan debug messenger!");
  }
}

#endif

int64_t getQueueFamilyIndex(VkPhysicalDevice device, VkQueueFlags flags) {
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  if(queueFamilyCount == 0) {
    return -1;
  }

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

  int64_t graphicsQueueFamilyIndex = -1;
  for (uint32_t i = 0; i < queueFamilies.size(); ++i) {
    VkQueueFamilyProperties props = queueFamilies[i];
    if (props.queueCount != 0 && props.queueFlags & flags) {
      graphicsQueueFamilyIndex = i;
      break;
    }
  }
  return graphicsQueueFamilyIndex;
}

inline int16_t devicePriorityByType(VkPhysicalDeviceType deviceType) {
  switch (deviceType) {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
      return 5;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
      return 4;
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
      return 3;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
      return 2;
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
      return 1;
    default:
      return -1;
  }
}

VkPhysicalDevice selectPhysicalDevice(VkInstance instance) {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
  if(deviceCount == 0) {
    ENJAM_ERROR("Failed to find GPUs with Vulkan support!");
    return VK_NULL_HANDLE;
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  struct DeviceInfo {
    VkPhysicalDevice device = VK_NULL_HANDLE;
    int16_t priority;
  };

  std::vector<DeviceInfo> deviceInfos;
  deviceInfos.reserve(devices.size());
  for(auto& device : devices) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    const int major = VK_API_VERSION_MAJOR(deviceProperties.apiVersion);
    const int minor = VK_API_VERSION_MINOR(deviceProperties.apiVersion);

    if(major < VULKAN_MINIMUM_REQUIRED_VERSION_MAJOR ||
        (major == VULKAN_MINIMUM_REQUIRED_VERSION_MAJOR && minor < VULKAN_MINIMUM_REQUIRED_VERSION_MINOR)) {
      continue;
    }

    auto graphicsQueueFamilyIndex = getQueueFamilyIndex(device, VK_QUEUE_GRAPHICS_BIT);
    if(graphicsQueueFamilyIndex < 0) {
      continue;
    }

    auto deviceExtensions = vulkan::utils::vkEnumerateDeviceExtensionProperties(device);
    bool supportSwapChain = deviceExtensions.find(VK_KHR_SWAPCHAIN_EXTENSION_NAME) != deviceExtensions.end();
    if(!supportSwapChain) {
      continue;
    }

    deviceInfos.push_back({
        .device = device,
        .priority = devicePriorityByType(deviceProperties.deviceType)
    });
  }

  std::sort(deviceInfos.begin(), deviceInfos.end(), [](auto& left, auto& right) {
    if (right.device == VK_NULL_HANDLE) {
      return false;
    }
    if (left.device == VK_NULL_HANDLE) {
      return true;
    }

    return left.priority < right.priority;
  });

  return deviceInfos.back().device;
}

VkShaderModule RendererBackendVulkan::createShaderModule(const ByteArray& code) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
  VkShaderModule shaderModule;
  if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
    ENJAM_ERROR("Failed to create shader module!");
    return VK_NULL_HANDLE;
  }
  return shaderModule;
}

void RendererBackendVulkan::createGraphicsPipeline(const ProgramData& data) {
  auto& sources = data.getSource();
  VkShaderModule vertShaderModule = createShaderModule(sources.at((size_t) ShaderStage::VERTEX));
  VkShaderModule fragShaderModule = createShaderModule(sources.at((size_t) ShaderStage::FRAGMENT));

  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
  vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
  fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

  std::vector<VkDynamicState> dynamicStates = {
      VK_DYNAMIC_STATE_VIEWPORT,
      VK_DYNAMIC_STATE_SCISSOR
  };

  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamicState.pDynamicStates = dynamicStates.data();



  vkDestroyShaderModule(device, fragShaderModule, nullptr);
  vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex) {
  float queuePriority = 1.0f;
  VkDeviceQueueCreateInfo queueCreateInfo {
    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
    .queueFamilyIndex = queueFamilyIndex,
    .queueCount = 1,
    .pQueuePriorities = &queuePriority,
  };

  VkPhysicalDeviceFeatures deviceFeatures { };

  auto deviceExtensions = vulkan::utils::vkEnumerateDeviceExtensionProperties(physicalDevice);
  const std::unordered_set<std::string_view> requestExtensions {
      VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
      VK_KHR_SWAPCHAIN_EXTENSION_NAME
  };

  std::unordered_set<std::string_view> enabledExtensions;
  for(auto& ext : requestExtensions) {
    if(deviceExtensions.find(ext) != deviceExtensions.end()) {
      enabledExtensions.insert(ext);
    }
  }

  void* pNext = nullptr;
  VkPhysicalDevicePortabilitySubsetFeaturesKHR portability = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR,
      .pNext = nullptr,
      .imageViewFormatSwizzle = VK_TRUE,
      .mutableComparisonSamplers = VK_TRUE,
  };
  if (enabledExtensions.find(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME) != enabledExtensions.end()) {
    portability.pNext = pNext;
    pNext = &portability;
  }

  std::vector<const char*> extensions(enabledExtensions.size());
  std::transform(enabledExtensions.cbegin(), enabledExtensions.cend(), extensions.begin(), [](auto& view) { return view.data(); });

  VkDeviceCreateInfo createInfo {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .pNext = pNext,
    .queueCreateInfoCount = 1,
    .pQueueCreateInfos = &queueCreateInfo,
    .enabledExtensionCount = (uint32_t) extensions.size(),
    .ppEnabledExtensionNames = extensions.data(),
    .pEnabledFeatures = &deviceFeatures
  };

  VkDevice ret;
  if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &ret) != VK_SUCCESS) {
    ENJAM_ERROR("Vulkan failed to create logical device!");
    return VK_NULL_HANDLE;
  }

  return ret;
}

VulkanSwapChain RendererBackendVulkan::createSwapChain() {
  auto availableFormats = vulkan::utils::vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface);

  ENJAM_ASSERT(!availableFormats.empty());

  auto surfaceFormat = std::find_if(
       availableFormats.begin(), availableFormats.end(),
       [](const VkSurfaceFormatKHR& format) {
         return format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
       }
  );
  ENJAM_ASSERT(surfaceFormat != availableFormats.end());

  auto presentModes = vulkan::utils::vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface);
  auto presentMode = std::find(presentModes.begin(), presentModes.end(), VK_PRESENT_MODE_FIFO_KHR);
  ENJAM_ASSERT(presentMode != presentModes.end());

  VkSurfaceCapabilitiesKHR capabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

  VkExtent2D extent;
  const uint32_t undefinedExtent = std::numeric_limits<uint32_t>::max();
  if(capabilities.currentExtent.width == undefinedExtent ||
     capabilities.currentExtent.height == undefinedExtent) {

    extent.width = std::clamp((uint32_t) frameBufferSize.x, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    extent.height = std::clamp((uint32_t) frameBufferSize.y, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

  } else {
    extent = capabilities.currentExtent;
  }

  uint32_t imageCount = capabilities.minImageCount + 1;
  if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
    imageCount = capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo;
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat->format;
  createInfo.imageColorSpace = surfaceFormat->colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  createInfo.preTransform = capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = *presentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  swapChain = { };
  if(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain.vkHandle) != VK_SUCCESS) {
    ENJAM_ERROR("Failed to create swap chain!");
    return swapChain;
  }

  swapChain.extent = extent;
  swapChain.imageFormat = surfaceFormat->format;
  swapChain.images = vulkan::utils::vkGetSwapchainImagesKHR(device, swapChain.vkHandle);
  return swapChain;
}

bool RendererBackendVulkan::init() {
#if ENJAM_VULKAN_ENABLED(ENJAM_VULKAN_DEBUG_UTILS)
  createDebugUtilsMessenger(instance, vkAlloc, &debugMessenger);
#endif

  physicalDevice = selectPhysicalDevice(instance);
  if(physicalDevice == VK_NULL_HANDLE) {
    ENJAM_ERROR("Vulkan failed to find a suitable GPU!");
  }

  auto deviceQueueFamilyIndex = (uint32_t) getQueueFamilyIndex(physicalDevice, VK_QUEUE_GRAPHICS_BIT);
  device = createLogicalDevice(physicalDevice, deviceQueueFamilyIndex);
  vkGetDeviceQueue(device, deviceQueueFamilyIndex, 0, &graphicsQueue);

  createSwapChain();

  return true;
}

void RendererBackendVulkan::shutdown() {
#if ENJAM_VULKAN_ENABLED(ENJAM_VULKAN_DEBUG_UTILS)
  if (debugMessenger) {
    vkDestroyDebugUtilsMessenger(instance, debugMessenger, nullptr);
  }
#endif

  vkDestroySwapchainKHR(device, swapChain.vkHandle, nullptr);
  vkDestroyDevice(device, nullptr);
  vkDestroySurfaceKHR(instance, surface, nullptr);
  vkDestroyInstance(instance, nullptr);
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