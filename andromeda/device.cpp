module andromeda;
import vulkan;

using namespace andromeda;

bool Application::isDeviceSuitable(
    const vk::raii::PhysicalDevice &physicalDevice) {
  bool supportsVulan13 =
      physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;
  auto queueFamilies = physicalDevice.getQueueFamilyProperties();
  bool supportsGraphics =
      std::ranges::any_of(queueFamilies, [](const auto &qfp) {
        return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
      });

  auto deviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
  bool supportsRequiredExtensions = std::ranges::all_of(
      requiredDeviceExtensions,
      [&deviceExtensions](const auto &requiredExtension) {
        return std::ranges::any_of(
            deviceExtensions, [requiredExtension](const auto &deviceExtension) {
              return std::strcmp(deviceExtension.extensionName,
                                 requiredExtension) == 0;
            });
      });

  auto features = physicalDevice.getFeatures2<
      vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features,
      vk::PhysicalDeviceVulkan13Features,
      vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
  bool supportsRequiredFeatures =
      features.get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
      features.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
      features.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>()
          .extendedDynamicState;
  return supportsVulan13 && supportsGraphics && supportsRequiredExtensions &&
         supportsRequiredFeatures;
}

void Application::selectPhysicalDevice() {
    std::vector<vk::raii::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();
    auto deviceIt = std::ranges::find_if(physicalDevices,[&](const auto& physicalDevice) {
        return isDeviceSuitable(physicalDevice);
    });

    if (deviceIt == physicalDevices.end()) {
        throw std::runtime_error{"Failed to find a suitable GPU!"};
    }
    physicalDevice = *deviceIt;
    auto deviceProperties = physicalDevice.getProperties();
    std::cout << std::format("Using device: {:s}\nDriver: {}\n", deviceProperties.deviceName, deviceProperties.driverVersion);
}