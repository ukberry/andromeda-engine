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

void Application::createLogicalDevice() {
    const std::vector<vk::QueueFamilyProperties>& qFamilyProperties {physicalDevice.getQueueFamilyProperties()};
    auto indices { std::views::iota(0uz, qFamilyProperties.size())};
    
    auto graphicsQFP = std::ranges::find_if(indices, [&](std::uint32_t idx) {
        const auto& qfp { qFamilyProperties[idx] };

        return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0) && physicalDevice.getSurfaceSupportKHR(idx, *surface);
    });

    queueIndex = (graphicsQFP != indices.end()) ? static_cast<std::uint32_t>(*graphicsQFP) : ~0u;
    if(queueIndex == ~0u) throw std::runtime_error{"Could not find a queue for graphics."};

    float queuePriority{ 0.5f };
    vk::StructureChain< vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features, vk::PhysicalDeviceVulkan13Features,
        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain {
            {},
            {.shaderDrawParameters = true},
            {.synchronization2 = true, .dynamicRendering = true},
            {.extendedDynamicState = true}
        };

        vk::DeviceQueueCreateInfo deviceQueueCreateInfo {
            .queueFamilyIndex = queueIndex,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority
        };
        vk::DeviceCreateInfo deviceCreateInfo{
            .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &deviceQueueCreateInfo,
            .enabledExtensionCount = static_cast<std::uint32_t>(requiredDeviceExtensions.size()),
            .ppEnabledExtensionNames = requiredDeviceExtensions.data()
        };

        std::uint32_t queueCount {0u};
        device = vk::raii::Device{physicalDevice, deviceCreateInfo};
        queue = vk::raii::Queue{device, queueIndex, queueCount};
}