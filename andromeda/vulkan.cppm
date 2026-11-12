module;
#include <vulkan/vk_platform.h>
export module andromeda:graphics_vulkan;

import glm;
import vulkan;
import std;
import :utility;

import :shaders;

#define VK_MAKE_VERSION(major,minor,patch) ((((uint32_t)(major)) << 22U) | (((uint32_t)(minor)) << 12U) | ((uint32_t)(patch)))

#ifdef NDEBUG
constexpr bool enableValidationLayers {false};
#else
constexpr bool enableValidationLayers {true};
#endif

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<char const*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

export namespace andromeda {

struct Vertex {
    glm::vec2 pos;
    glm::vec3 colour;

    static vk::VertexInputBindingDescription GetBindingDescription() {
    return {.binding = 0, .stride = sizeof(Vertex), .inputRate = vk::VertexInputRate::eVertex};
    }

    static std::array<vk::VertexInputAttributeDescription,2> GetAttributeDescription() {
        return {{
            {.location = 0, .binding = 0, .format = vk::Format::eR32G32Sfloat, .offset = offsetof(Vertex, pos)},
            {.location = 1, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(Vertex, colour)}
        }};
    }
};

template <class Context>
class VulkanGraphicsPolicy {
public:
    using is_graphics_policy = void;

    bool InitGraphics() {
        CreateInstance();
        SetupDebugMessages();
        surface = static_cast<Context*>(this)->CreateSurface();
        SelectPhysicalDevice();
        CreateLogicalDevice();
        CreateSwapChain();
        CreateImageViews();
        CreateGraphicsPipeline();

        if (surface != nullptr) return true;
        return false;
    }

    void CreateInstance() {
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
        (assert)(layerProperties.has_value(), "Unable to ascertain layer properties");
        auto unsupportedIt = std::ranges::find_if(requiredLayers, [&layerProperties](const auto& requiredLayer) {
            return std::ranges::none_of(layerProperties.value, [requiredLayer](const auto& layerProperty) {
                return std::strcmp(layerProperty.layerName, requiredLayer) == 0;
            });
        });

        if (unsupportedIt != requiredLayers.end()) {
            std::cerr << "Required layer not supported: " + std::string{*unsupportedIt} << std::endl;
            std::terminate();
        }

        auto requiredExtensions { static_cast<Context*>(this)->GetRequiredWindowExtensions() };
        if (enableValidationLayers) requiredExtensions.push_back(vk::EXTDebugUtilsExtensionName);

        const auto extensionProperties { context.enumerateInstanceExtensionProperties() };
        (assert)(extensionProperties.has_value(), "Unable to determine extension properties");
        unsupportedIt = std::ranges::find_if(requiredExtensions, [&extensionProperties](const auto& requiredExtension){
            return std::ranges::none_of(extensionProperties.value,[requiredExtension](const auto& extensionProperty) {
                return std::strcmp(extensionProperty.extensionName, requiredExtension) == 0;
            });
        });

        if (unsupportedIt != requiredExtensions.end()) {
            std::cerr << "Required extension not supported: " + std::string{*unsupportedIt} << std::endl;
            std::terminate();
        }

        vk::InstanceCreateInfo createInfo{
            .pApplicationInfo = &appInfo,
            .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
            .ppEnabledLayerNames = requiredLayers.data(),
            .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
            .ppEnabledExtensionNames = requiredExtensions.data()
        };
        //instance = vk::raii::Instance{context, createInfo};
        auto instance_create {context.createInstance(createInfo)};
        (assert)(instance_create.has_value(), "Could not create vulkan instance");
        instance = std::move(*instance_create);
    }

    VKAPI_ATTR static vk::Bool32 VKAPI_CALL DebugCallback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
    vk::DebugUtilsMessageTypeFlagsEXT type,
    const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, void *) {
  if (severity > vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
    std::cerr << "Validation layer: type " << to_string(type)
              << " msg: " << pCallbackData->pMessage << std::endl;
  }
  return vk::False;
}

void SetupDebugMessages() {
  if (!enableValidationLayers)
    return;

  using Severity = vk::DebugUtilsMessageSeverityFlagBitsEXT;
  using Type = vk::DebugUtilsMessageTypeFlagBitsEXT;
  vk::DebugUtilsMessageSeverityFlagsEXT severityFlags{Severity::eWarning |
                                                      Severity::eError};
  vk::DebugUtilsMessageTypeFlagsEXT typeFlags{
      Type::eGeneral | Type::ePerformance | Type::eValidation};

  vk::DebugUtilsMessengerCreateInfoEXT createInfo{
      .messageSeverity = severityFlags, .messageType = typeFlags,
      .pfnUserCallback = &DebugCallback
  };
  auto result {instance.createDebugUtilsMessengerEXT(createInfo)};
  (assert)(result.has_value(), "Could not create a debug messenger");
  debugMessenger = std::move(*result);
}

/// DEVICE FUNCTIONS ///
bool IsDeviceSuitable(
    const vk::raii::PhysicalDevice &physicalDevice) {
  bool supportsVulan13 =
      physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;
  auto queueFamilies = physicalDevice.getQueueFamilyProperties();
  bool supportsGraphics =
      std::ranges::any_of(queueFamilies, [](const auto &qfp) {
        return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
      });

  auto deviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
  assert(deviceExtensions.has_value());
  bool supportsRequiredExtensions = std::ranges::all_of(
      requiredDeviceExtensions,
      [&deviceExtensions](const auto &requiredExtension) {
        return std::ranges::any_of(
            deviceExtensions.value, [requiredExtension](const auto &deviceExtension) {
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

void SelectPhysicalDevice() {
    auto [result,physicalDevices] = instance.enumeratePhysicalDevices();
    (assert)(result == vk::Result::eSuccess,"Enumerate physical devices failed.");
    auto deviceIt = std::ranges::find_if(physicalDevices,[&](const auto& physicalDevice) {
        return IsDeviceSuitable(physicalDevice);
    });

    if (deviceIt == physicalDevices.end()) {
        std::cerr << "Failed to find a suitable GPU!" << std::endl;
        std::terminate();
    }
    physicalDevice = *deviceIt;
    auto deviceProperties = physicalDevice.getProperties();
    std::cout << std::format("Using device: {:s}\nDriver: {}\n", deviceProperties.deviceName, deviceProperties.driverVersion);
}

void CreateLogicalDevice() {
    const std::vector<vk::QueueFamilyProperties>& qFamilyProperties {physicalDevice.getQueueFamilyProperties()};
    auto indices { std::views::iota(0uz, qFamilyProperties.size())};
    
    auto graphicsQFP = std::ranges::find_if(indices, [&](std::uint32_t idx) {
        const auto& qfp { qFamilyProperties[idx] };
        auto surface_support = physicalDevice.getSurfaceSupportKHR(idx, *surface);
        return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0) && surface_support.has_value();
    });

    queueIndex = (graphicsQFP != indices.end()) ? static_cast<std::uint32_t>(*graphicsQFP) : ~0u;
    if(queueIndex == ~0u) {
        std::cerr << "Could not find a suitable queue for graphics" << std::endl;
        std::terminate();
    };

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
        auto [result,device_create] = physicalDevice.createDevice(deviceCreateInfo);
        (assert)(result == vk::Result::eSuccess, "Failed to created a physical device reference");
        device = std::move(device_create);
        queue = device.getQueue(queueIndex, queueCount);
    }

    vk::raii::Instance& GetVulkanInstance() {
        return instance;
    }

private:
    bool isLoaded{};
        // Vulkan setup
    vk::raii::Context context;
    vk::raii::Instance instance {nullptr};
    vk::raii::DebugUtilsMessengerEXT debugMessenger {nullptr};

    // Window surface setup
    vk::raii::SurfaceKHR surface {nullptr};

    // Device setup
    vk::raii::PhysicalDevice physicalDevice {nullptr};
    vk::raii::Device device {nullptr};
    uint32_t queueIndex {~0u};
    vk::raii::Queue queue {nullptr};

    // Swapchain setup
    vk::raii::SwapchainKHR swapChain {nullptr};
    vk::Extent2D swapChainExtent;
    vk::SurfaceFormatKHR swapChainSurfaceFormat;
    std::vector<vk::Image> swapChainImages;
    std::vector<vk::raii::ImageView> swapChainImageViews;

    std::vector<const char*> requiredDeviceExtensions {
        vk::KHRSwapchainExtensionName
    };

    vk::raii::DescriptorSetLayout descriptorSetLayout {nullptr};
    vk::raii::PipelineLayout pipelineLayout {nullptr};
    vk::raii::Pipeline graphicsPipeline {nullptr};

    vk::raii::CommandPool commandPool {nullptr};
    std::vector<vk::raii::CommandBuffer> commandBuffers;

    /// Swapchain functions ///
    void CreateSwapChain() {
    const vk::SurfaceCapabilitiesKHR surfaceCapabilities { physicalDevice.getSurfaceCapabilitiesKHR(*surface).value };
    swapChainExtent = ChooseSwapExtent(surfaceCapabilities);

    std::uint32_t minImageCount = ChooseSwapMinImageCount(surfaceCapabilities);

    const std::vector<vk::SurfaceFormatKHR>& availableFormats { physicalDevice.getSurfaceFormatsKHR(*surface).value };
    swapChainSurfaceFormat = ChooseSwapSurfaceFormat(availableFormats);

    const std::vector<vk::PresentModeKHR>& availablePresentModes { physicalDevice.getSurfacePresentModesKHR(*surface).value };
    vk::PresentModeKHR presentMode { ChooseSwapPresentMode(availablePresentModes) };

    vk::SwapchainCreateInfoKHR swapChainCreateInfo {
        .surface = *surface,
        .minImageCount = minImageCount,
        .imageFormat = swapChainSurfaceFormat.format,
        .imageColorSpace = swapChainSurfaceFormat.colorSpace,
        .imageExtent = swapChainExtent,
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .preTransform = surfaceCapabilities.currentTransform,
        .presentMode = presentMode,
        .clipped = true
    };
    //swapChain = vk::raii::SwapchainKHR{device, swapChainCreateInfo};
    auto swapchain_create {device.createSwapchainKHR(swapChainCreateInfo)};
    (assert)(swapchain_create.has_value(),"Could not create a swap chain");
    swapChain = std::move(*swapchain_create);
    auto images_create {swapChain.getImages()};
    (assert)(images_create.has_value(),"Unable to get swapchain images");
    swapChainImages = std::move(*images_create);   
}

void CreateImageViews() {
    assert (swapChainImageViews.empty());

    constexpr std::uint32_t baseMipLevel {0};
    constexpr std::uint32_t levelCount {1};
    constexpr std::uint32_t baseArrayLayer {0};
    constexpr std::uint32_t layerCount {1};

    vk::ImageViewCreateInfo imageViewCreateInfo{
        .viewType = vk::ImageViewType::e2D,
        .format = swapChainSurfaceFormat.format,
        .subresourceRange = {vk::ImageAspectFlagBits::eColor, baseMipLevel, levelCount,
            baseArrayLayer, layerCount}
    };

    for (auto& image : swapChainImages) {
        imageViewCreateInfo.image = image;
        auto image_view {device.createImageView(imageViewCreateInfo)};
        (assert)(image_view.has_value(), "Could not create an ImageView");
        swapChainImageViews.push_back(std::move(*image_view));
    }
}

vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const {
    if (capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max()) {
        return capabilities.currentExtent;
    }

    auto [width, height] = static_cast<const Context*>(this)->GetWindowSize();

    return {
        std::clamp<std::uint32_t>(static_cast<std::uint32_t>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
        std::clamp<std::uint32_t>(static_cast<std::uint32_t>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
    };
}

std::uint32_t ChooseSwapMinImageCount(const vk::SurfaceCapabilitiesKHR& capabilities) {
    auto minImageCount { std::max(3u, capabilities.maxImageCount) };
    
    if (capabilities.maxImageCount > 0 && capabilities.maxImageCount < minImageCount) minImageCount = capabilities.maxImageCount;

    return minImageCount;
}

vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {

    // We ensure we have at least one format
    (assert) (!availableFormats.empty(),"Cannot choose SwapSurfaceFormat from empty container.");

    const auto formatIt = std::ranges::find_if(availableFormats,[](const vk::SurfaceFormatKHR& format) {
        return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
    });
    return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
}

vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
    
    // eFifo must be guaranteed by the implementation
    (assert) (std::ranges::any_of(availablePresentModes,[](const vk::PresentModeKHR presentMode){
        return presentMode == vk::PresentModeKHR::eFifo;
    }),"Vulkan implementation must implement eFifo present mode.");

    // We prefer a Mailbox presentation, but we'll fallback if necessary
    return std::ranges::any_of(availablePresentModes,[](const vk::PresentModeKHR presentMode) {
        return presentMode == vk::PresentModeKHR::eMailbox;
    }) ? vk::PresentModeKHR::eMailbox : vk::PresentModeKHR::eFifo;

}

void CreateDescriptorSetLayout () {
    vk::DescriptorSetLayoutBinding uboLayoutBinding {
        .binding = 0, .descriptorType = vk::DescriptorType::eUniformBuffer,
        .descriptorCount = 1, .stageFlags = vk::ShaderStageFlagBits::eVertex
    };
    vk::DescriptorSetLayoutCreateInfo layoutInfo {
        .bindingCount = 1,
        .pBindings = &uboLayoutBinding
    };
    auto descriptorSetLayoutCreate = device.createDescriptorSetLayout(layoutInfo);
    (assert)(descriptorSetLayoutCreate.has_value(), "Could not create descriptor set layout");
    descriptorSetLayout = std::move(descriptorSetLayoutCreate.value);

}



/// TEMP DATA ///
const std::vector<Vertex> vertices  {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};
const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};
/// END TEMP DATA ///

void CreateGraphicsPipeline() {
    ResourceManager rm;
    auto shader {rm.Load<Shader>("default_shader", std::addressof(device), "shaders/slang.spv")};

    vk::PipelineShaderStageCreateInfo vertShaderCreateInfo{
        .stage = vk::ShaderStageFlagBits::eVertex, .module = shader->GetShaderModule() , .pName = "vertMain"
    };
    vk::PipelineShaderStageCreateInfo fragShaderCreateInfo{
        .stage = vk::ShaderStageFlagBits::eFragment, .module = shader->GetShaderModule() , .pName = "fragMain"
    };
    vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderCreateInfo, fragShaderCreateInfo};

    auto bindingDescription {Vertex::GetBindingDescription()};
    auto attributeDescription {Vertex::GetAttributeDescription()};
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo {
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &bindingDescription,
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size()),
        .pVertexAttributeDescriptions = attributeDescription.data()
    };
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly {.topology = vk::PrimitiveTopology::eTriangleList};
    vk::PipelineViewportStateCreateInfo viewportState{.viewportCount = 1, .scissorCount = 1};

    vk::PipelineRasterizationStateCreateInfo rasteriser {
        .depthClampEnable = vk::False,
        .rasterizerDiscardEnable = vk::False,
        .polygonMode = vk::PolygonMode::eFill,
        .cullMode = vk::CullModeFlagBits::eBack,
        .frontFace = vk::FrontFace::eCounterClockwise,
        .depthBiasEnable = vk::False,
        .lineWidth = 1.0f
    };

    vk::PipelineMultisampleStateCreateInfo multisampling {
        .rasterizationSamples = vk::SampleCountFlagBits::e1, .sampleShadingEnable = vk::False
    };
    vk::PipelineColorBlendAttachmentState colourBlendAttachment {
        .blendEnable = vk::False,
        .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
             vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
    };
    vk::PipelineColorBlendStateCreateInfo colourBlending{
        .logicOpEnable = vk::False, .logicOp = vk::LogicOp::eCopy, .attachmentCount = 1, .pAttachments = &colourBlendAttachment
    };

    std::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    vk::PipelineDynamicStateCreateInfo dynamicStateInfo {
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data()
    };

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo {
        .setLayoutCount = 0, .pushConstantRangeCount = 0
    };
    auto pipelineLayoutCreate = device.createPipelineLayout(pipelineLayoutInfo);
    (assert)(pipelineLayoutCreate.has_value(), "Pipeline layout creation failed");
    pipelineLayout = std::move(*pipelineLayoutCreate);

    vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain = {
        {
            .stageCount = 2,
            .pStages = shaderStages,
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssembly,
            .pViewportState = &viewportState,
            .pRasterizationState = &rasteriser,
            .pMultisampleState = &multisampling,
            .pColorBlendState = &colourBlending,
            .pDynamicState = &dynamicStateInfo,
            .layout = pipelineLayout,
            .renderPass = nullptr
        },
        {
            .colorAttachmentCount = 1,
            .pColorAttachmentFormats = &swapChainSurfaceFormat.format
        }
    };
    auto graphicsPipelineCreate = device.createGraphicsPipeline(nullptr,
        pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());
    (assert)(graphicsPipelineCreate.has_value(), "Failed to create the graphics pipeline");
    graphicsPipeline = std::move(*graphicsPipelineCreate);
}

void CreateCommandPool() {
    vk::CommandPoolCreateInfo poolInfo{
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = queueIndex
    };
    
    auto commandPoolCreate = device.createCommandPool(poolInfo);
    (assert)(commandPoolCreate.has_value(), "Could not create command pool");
    commandPool = std::move(commandPoolCreate.value);
}

void CreateCommandBuffers() {
    vk::CommandBufferAllocateInfo allocInfo {
        .commandPool = commandPool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = MAX_FRAMES_IN_FLIGHT
    };
    
    auto commandBuffersCreate = device.allocateCommandBuffers(allocInfo);
    (assert)(commandBuffersCreate.has_value(), "Could not allocate command buffers");
    commandBuffers = std::move(commandBuffersCreate.value);
}

std::uint32_t FindMemoryType(std::uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    
    vk::PhysicalDeviceMemoryProperties memProperties {physicalDevice.getMemoryProperties()};

    for (std::uint32_t i{}; i < memProperties.memoryTypeCount; i++) {
        if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }
}

std::pair <vk::raii::Buffer, vk::raii::DeviceMemory>
    CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) {

    vk::BufferCreateInfo bufferInfo {
        .size = size, .usage = usage, .sharingMode = vk::SharingMode::eExclusive
    };
    auto buffer { device.createBuffer(bufferInfo) };
    (assert)(buffer.has_value(), "Could not create buffer");
    
    vk::MemoryRequirements memRequirements {buffer->getMemoryRequirements()};
    vk::MemoryAllocateInfo allocInfo {
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties)
    };
    auto bufferMemory { device.allocateMemory(allocInfo) };
    (assert)(bufferMemory.has_value(), "Could not allocate device memory");
    buffer->bindMemory(*bufferMemory.value, 0);
    return {std::move(buffer.value), std::move(bufferMemory.value)};
}

};

} // namespace: andromeda
