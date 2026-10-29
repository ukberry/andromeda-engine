module andromeda;
import vulkan;

using namespace andromeda;

// bool Application::isDeviceSuitable(
//     const vk::raii::PhysicalDevice &physicalDevice) {
//   bool supportsVulan13 =
//       physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;
//   auto queueFamilies = physicalDevice.getQueueFamilyProperties();
//   bool supportsGraphics =
//       std::ranges::any_of(queueFamilies, [](const auto &qfp) {
//         return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
//       });

//   auto deviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
//   assert(deviceExtensions.has_value());
//   bool supportsRequiredExtensions = std::ranges::all_of(
//       requiredDeviceExtensions,
//       [&deviceExtensions](const auto &requiredExtension) {
//         return std::ranges::any_of(
//             deviceExtensions.value, [requiredExtension](const auto &deviceExtension) {
//               return std::strcmp(deviceExtension.extensionName,
//                                  requiredExtension) == 0;
//             });
//       });

//   auto features = physicalDevice.getFeatures2<
//       vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features,
//       vk::PhysicalDeviceVulkan13Features,
//       vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
//   bool supportsRequiredFeatures =
//       features.get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
//       features.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
//       features.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>()
//           .extendedDynamicState;
//   return supportsVulan13 && supportsGraphics && supportsRequiredExtensions &&
//          supportsRequiredFeatures;
// }

// void Application::selectPhysicalDevice() {
//     auto [result,physicalDevices] = instance.enumeratePhysicalDevices();
//     assert(result == vk::Result::eSuccess,"Enumerate physical devices failed.");
//     auto deviceIt = std::ranges::find_if(physicalDevices,[&](const auto& physicalDevice) {
//         return isDeviceSuitable(physicalDevice);
//     });

//     if (deviceIt == physicalDevices.end()) {
//         std::cerr << "Failed to find a suitable GPU!" << std::endl;
//         std::terminate();
//     }
//     physicalDevice = *deviceIt;
//     auto deviceProperties = physicalDevice.getProperties();
//     std::cout << std::format("Using device: {:s}\nDriver: {}\n", deviceProperties.deviceName, deviceProperties.driverVersion);
// }

// void Application::createLogicalDevice() {
//     const std::vector<vk::QueueFamilyProperties>& qFamilyProperties {physicalDevice.getQueueFamilyProperties()};
//     auto indices { std::views::iota(0uz, qFamilyProperties.size())};
    
//     auto graphicsQFP = std::ranges::find_if(indices, [&](std::uint32_t idx) {
//         const auto& qfp { qFamilyProperties[idx] };
//         auto surface_support = physicalDevice.getSurfaceSupportKHR(idx, *surface);
//         return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0) && surface_support.has_value();
//     });

//     queueIndex = (graphicsQFP != indices.end()) ? static_cast<std::uint32_t>(*graphicsQFP) : ~0u;
//     if(queueIndex == ~0u) {
//         std::cerr << "Could not find a suitable queue for graphics" << std::endl;
//         std::terminate();
//     };

//     float queuePriority{ 0.5f };
//     vk::StructureChain< vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features, vk::PhysicalDeviceVulkan13Features,
//         vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain {
//             {},
//             {.shaderDrawParameters = true},
//             {.synchronization2 = true, .dynamicRendering = true},
//             {.extendedDynamicState = true}
//         };

//         vk::DeviceQueueCreateInfo deviceQueueCreateInfo {
//             .queueFamilyIndex = queueIndex,
//             .queueCount = 1,
//             .pQueuePriorities = &queuePriority
//         };
//         vk::DeviceCreateInfo deviceCreateInfo{
//             .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
//             .queueCreateInfoCount = 1,
//             .pQueueCreateInfos = &deviceQueueCreateInfo,
//             .enabledExtensionCount = static_cast<std::uint32_t>(requiredDeviceExtensions.size()),
//             .ppEnabledExtensionNames = requiredDeviceExtensions.data()
//         };

//         std::uint32_t queueCount {0u};
//         auto [result,device_create] = physicalDevice.createDevice(deviceCreateInfo);
//         assert(result == vk::Result::eSuccess, "Failed to created a physical device reference");
//         device = std::move(device_create);
//         queue = device.getQueue(queueIndex, queueCount);
// }