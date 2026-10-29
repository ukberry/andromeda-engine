module andromeda;
import vulkan;

using namespace andromeda;

// void Application::createSwapChain() {
//     const vk::SurfaceCapabilitiesKHR surfaceCapabilities { physicalDevice.getSurfaceCapabilitiesKHR(*surface).value };
//     swapChainExtent = chooseSwapExtent(surfaceCapabilities);

//     std::uint32_t minImageCount = chooseSwapMinImageCount(surfaceCapabilities);

//     const std::vector<vk::SurfaceFormatKHR>& availableFormats { physicalDevice.getSurfaceFormatsKHR(*surface).value };
//     swapChainSurfaceFormat = chooseSwapSurfaceFormat(availableFormats);

//     const std::vector<vk::PresentModeKHR>& availablePresentModes { physicalDevice.getSurfacePresentModesKHR(*surface).value };
//     vk::PresentModeKHR presentMode { chooseSwapPresentMode(availablePresentModes) };

//     vk::SwapchainCreateInfoKHR swapChainCreateInfo {
//         .surface = *surface,
//         .minImageCount = minImageCount,
//         .imageFormat = swapChainSurfaceFormat.format,
//         .imageColorSpace = swapChainSurfaceFormat.colorSpace,
//         .imageExtent = swapChainExtent,
//         .imageArrayLayers = 1,
//         .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
//         .imageSharingMode = vk::SharingMode::eExclusive,
//         .preTransform = surfaceCapabilities.currentTransform,
//         .presentMode = presentMode,
//         .clipped = true
//     };
//     //swapChain = vk::raii::SwapchainKHR{device, swapChainCreateInfo};
//     auto swapchain_create {device.createSwapchainKHR(swapChainCreateInfo)};
//     assert(swapchain_create.has_value(),"Could not create a swap chain");
//     swapChain = std::move(*swapchain_create);
//     auto images_create {swapChain.getImages()};
//     assert(images_create.has_value(),"Unable to get swapchain images");
//     swapChainImages = std::move(*images_create);   
// }

// void Application::createImageViews() {
//     assert (swapChainImageViews.empty());

//     constexpr std::uint32_t baseMipLevel {0};
//     constexpr std::uint32_t levelCount {1};
//     constexpr std::uint32_t baseArrayLayer {0};
//     constexpr std::uint32_t layerCount {1};

//     vk::ImageViewCreateInfo imageViewCreateInfo{
//         .viewType = vk::ImageViewType::e2D,
//         .format = swapChainSurfaceFormat.format,
//         .subresourceRange = {vk::ImageAspectFlagBits::eColor, baseMipLevel, levelCount,
//             baseArrayLayer, layerCount}
//     };

//     for (auto& image : swapChainImages) {
//         imageViewCreateInfo.image = image;
//         auto image_view {device.createImageView(imageViewCreateInfo)};
//         assert(image_view.has_value(), "Could not create an ImageView");
//         swapChainImageViews.push_back(std::move(*image_view));
//     }
// }

// vk::Extent2D Application::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const {
//     if (capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max()) {
//         return capabilities.currentExtent;
//     }

//     auto [width, height] = getWindowSize();

//     return {
//         std::clamp<std::uint32_t>(static_cast<std::uint32_t>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
//         std::clamp<std::uint32_t>(static_cast<std::uint32_t>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
//     };
// }

// std::uint32_t Application::chooseSwapMinImageCount(const vk::SurfaceCapabilitiesKHR& capabilities) {
//     auto minImageCount { std::max(3u, capabilities.maxImageCount) };
    
//     if (capabilities.maxImageCount > 0 && capabilities.maxImageCount < minImageCount) minImageCount = capabilities.maxImageCount;

//     return minImageCount;
// }

// vk::SurfaceFormatKHR Application::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {

//     // We ensure we have at least one format
//     assert (!availableFormats.empty(),"Cannot choose SwapSurfaceFormat from empty container.");

//     const auto formatIt = std::ranges::find_if(availableFormats,[](const vk::SurfaceFormatKHR& format) {
//         return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
//     });
//     return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
// }

// vk::PresentModeKHR Application::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
    
//     // eFifo must be guaranteed by the implementation
//     assert (std::ranges::any_of(availablePresentModes,[](const vk::PresentModeKHR presentMode){
//         return presentMode == vk::PresentModeKHR::eFifo;
//     }),"Vulkan implementation must implement eFifo present mode.");

//     // We prefer a Mailbox presentation, but we'll fallback if necessary
//     return std::ranges::any_of(availablePresentModes,[](const vk::PresentModeKHR presentMode) {
//         return presentMode == vk::PresentModeKHR::eMailbox;
// //     }) ? vk::PresentModeKHR::eMailbox : vk::PresentModeKHR::eFifo;

// }