module andromeda;

import vulkan;
import std.compat;

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

    const auto layerProperties { context.enumerateInstanceLayerProperties() };
    auto unsupportedIt = std::ranges::find_if(requiredLayers, [&layerProperties](const auto& requiredLayer) {
        return std::ranges::none_of(layerProperties, [requiredLayer](const auto& layerProperty) {
            return std::strcmp(layerProperty.layerName, requiredLayer) == 0;
        });
    });

    if (unsupportedIt != requiredLayers.end()) {
        throw std::runtime_error{"Required layer not supported: " + std::string{*unsupportedIt}};
    }

    auto requiredExtensions { getRequiredGLFWExtensions() };
    if (enableValidationLayers) requiredExtensions.push_back(vk::EXTDebugUtilsExtensionName);

    const auto extensionProperties { context.enumerateInstanceExtensionProperties() };
    unsupportedIt = std::ranges::find_if(requiredExtensions, [&extensionProperties](const auto& requiredExtension){
        return std::ranges::none_of(extensionProperties,[requiredExtension](const auto& extensionProperty) {
            return std::strcmp(extensionProperty.extensionName, requiredExtension) == 0;
        });
    });

    if (unsupportedIt != requiredExtensions.end()) {
        throw std::runtime_error("Required extension not supported: " + std::string{*unsupportedIt});
    }

    vk::InstanceCreateInfo createInfo{
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
        .ppEnabledLayerNames = requiredLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
        .ppEnabledExtensionNames = requiredExtensions.data()
    };
    instance = vk::raii::Instance{context, createInfo};
}


