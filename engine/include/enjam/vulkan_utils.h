#ifndef ENGINE_INCLUDE_ENJAM_VULKAN_UTILS_H_
#define ENGINE_INCLUDE_ENJAM_VULKAN_UTILS_H_

#include <vulkan/vulkan.h>

namespace Enjam::vulkan::utils {

namespace {

template<class T, class...TArgs>
std::vector<T> enumerate(VkResult (*func)(TArgs..., uint32_t*, T*), TArgs... args) {
  uint32_t count;
  func(args..., &count, nullptr);

  if(count <= 0) {
    return { };
  }

  std::vector<T> ret(count);
  if(func(args..., &count, ret.data()) == VK_SUCCESS) {
    return ret;
  }

  return { };
}

}

auto vkGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
  return enumerate<VkSurfaceFormatKHR>(::vkGetPhysicalDeviceSurfaceFormatsKHR, physicalDevice, surface);
}

auto vkGetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
  return enumerate<VkPresentModeKHR>(::vkGetPhysicalDeviceSurfacePresentModesKHR, physicalDevice, surface);
}

auto vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice) {
  auto vec = enumerate<VkExtensionProperties>(::vkEnumerateDeviceExtensionProperties, physicalDevice, (const char*) nullptr);
  std::unordered_set<std::string_view> set;
  for(auto& prop : vec) {
    set.emplace(prop.extensionName);
  }
  return set;
}

auto vkGetSwapchainImagesKHR(VkDevice device, VkSwapchainKHR swapchain) {
  return enumerate<VkImage>(::vkGetSwapchainImagesKHR, device, swapchain);
}

}

#endif //ENGINE_INCLUDE_ENJAM_VULKAN_UTILS_H_
