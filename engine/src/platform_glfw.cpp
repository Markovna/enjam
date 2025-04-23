#include <enjam/platform_glfw.h>

#include <set>
#include <enjam/vulkan_defines.h>
#include <enjam/input_events.h>
#include <enjam/input.h>
#include <enjam/assert.h>
#include <enjam/renderer_backend_opengl.h>
#include <enjam/renderer_backend_vulkan.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

namespace Enjam {

PlatformGlfw::PlatformGlfw()
  : initialized(false) {
  init();
}

template<class InputIterator, class OutputIterator>
void vkAvailableLayers(InputIterator first, InputIterator second, OutputIterator output) {
  using InputValueType = typename std::iterator_traits<InputIterator>::value_type;

  static_assert(std::is_assignable<decltype(std::declval<OutputIterator>().operator*()), InputValueType>::value);
  static_assert(std::is_constructible<std::string_view, InputValueType>::value);

  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for(auto it = first; it != second; it++) {
    for (const VkLayerProperties& layer: availableLayers) {
      const std::string_view availableLayer(layer.layerName);
      if (availableLayer != std::string_view { *it }) {
        continue;
      }

      *output = *it;
      output++;
    }
  }
}

std::unique_ptr<RendererBackend> PlatformGlfw::createRendererBackend(RendererBackendType type) {
  ENJAM_ASSERT(window);

  switch (type) {
    case DEFAULT:
    case OPENGL: {
      GLSwapChain swapChain {
          .makeCurrent = [win = window]() { glfwMakeContextCurrent(win); },
          .swapBuffers = [win = window]() { glfwSwapBuffers(win); }
      };
      return std::make_unique<RendererBackendOpengl>((GLLoaderProc) glfwGetProcAddress, swapChain);
    }
    case VULKAN: {
      std::set<std::string_view> requiredExtensions;
      uint32_t glfwExtensionCount = 0;
      const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
      for(uint32_t i = 0; i < glfwExtensionCount; i++) {
        requiredExtensions.insert(glfwExtensions[i]);
      }

      requiredExtensions.insert(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

      std::vector<const char*> enabledLayers;
#if ENJAM_VULKAN_ENABLED(ENJAM_VULKAN_VALIDATION)
      const char* desiredLayers[] = {
          "VK_LAYER_KHRONOS_validation"
      };

      vkAvailableLayers(std::begin(desiredLayers), std::end(desiredLayers), std::back_inserter(enabledLayers));

      if(!enabledLayers.empty()) {
        requiredExtensions.insert(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
      }
#endif

#if ENJAM_VULKAN_ENABLED(ENJAM_VULKAN_DEBUG_UTILS)
      requiredExtensions.insert(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

      std::vector<const char*> extensions(requiredExtensions.size());
      std::transform(requiredExtensions.cbegin(), requiredExtensions.cend(), extensions.begin(),
                     [](auto& e) { return e.data(); });

      VkApplicationInfo appInfo {
          .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
          .pEngineName = "Enjam",
          .apiVersion = VK_API_VERSION_1_0
      };

      VkInstanceCreateInfo createInfo {
          .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
          .pApplicationInfo = &appInfo,
          .enabledLayerCount = (uint32_t) enabledLayers.size(),
          .ppEnabledLayerNames = enabledLayers.empty() ? nullptr : enabledLayers.data(),
          .enabledExtensionCount = (uint32_t) extensions.size(),
          .ppEnabledExtensionNames = extensions.data(),
      };

      if(requiredExtensions.find(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) != requiredExtensions.end()) {
        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
      }

      VkInstance instance;
      if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        ENJAM_ERROR("Failed to create VULKAN renderer backend!");
        return { };
      }

      return std::make_unique<RendererBackendVulkan>(std::move(instance));
    }
    case DIRECTX:
      ENJAM_ERROR("DIRECTX renderer backend is not supported for current platform.");
      return { };
  }
}

void PlatformGlfw::init() {
  ENJAM_ASSERT(!initialized);

  int status = glfwInit();
  ENJAM_ASSERT(status == GLFW_TRUE);

  initialized = true;

  glfwSetErrorCallback([](int error_code, const char* description) {
    ENJAM_ERROR("GLFW error: {} (Code: {})", description, error_code);
  });

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  window = glfwCreateWindow(800, 640, "Enjam", NULL, NULL);
  ENJAM_ASSERT(window != nullptr);

  glfwMakeContextCurrent(window);

  // set input callbacks
  glfwSetKeyCallback(window, [](GLFWwindow *w, int key, int scancode, int action, int mods) {
    auto input = (Input*) glfwGetWindowUserPointer(w);
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
      input->onKeyPress().call(
          KeyPressEventArgs {
              .keyCode = KeyCode(key),
              .control = bool(mods & GLFW_MOD_CONTROL),
              .shift = bool(mods & GLFW_MOD_SHIFT),
              .alt = bool(mods & GLFW_MOD_ALT),
              .super = bool(mods & GLFW_MOD_SUPER),
              .repeat = action == GLFW_REPEAT
          });
    }
    else if (action == GLFW_RELEASE) {
      input->onKeyRelease().call(
          KeyReleaseEventArgs {
              .keyCode = KeyCode(key),
              .control = bool(mods & GLFW_MOD_CONTROL),
              .shift = bool(mods & GLFW_MOD_SHIFT),
              .alt = bool(mods & GLFW_MOD_ALT),
              .super = bool(mods & GLFW_MOD_SUPER)
          });
    }
  });
}

void PlatformGlfw::pollInputEvents(Input& input) {
  glfwSetWindowUserPointer(window, &input);
  glfwPollEvents();
  glfwSetWindowUserPointer(window, nullptr);
}

void PlatformGlfw::shutdown() {
  glfwTerminate();
}

}