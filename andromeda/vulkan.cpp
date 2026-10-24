module;
#include <cstdint>
module andromeda;

import vulkan;

#define VK_MAKE_VERSION(major,minor,patch) ((((uint32_t)(major)) << 22U) | (((uint32_t)(minor)) << 12U) | ((uint32_t)(patch)))

#ifdef NDEBUG
constexpr bool enableValidationLayers {false};
#else
constexpr bool enableValidationLayers {true};
#endif

const std::vector<char const*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

using namespace andromeda;

void Application::initVulkan() {
    createInstance();
}

void Application::createInstance() {
    constexpr vk::ApplicationInfo appInfo{
        .pApplicationName = "Andromeda Application",
        .applicationVersion = VK_MAKE_VERSION(1,0,0),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(1,0,0),
        .apiVersion = vk::ApiVersion14
    };

    std::vector<const char*> requiredLayers;
    if (enableValidationLayers) {
        requiredLayers.assign(validationLayers.begin(), validationLayers.end());
    }
}
