module;
#include <GLFW/glfw3.h>
#include <vulkan/vk_platform.h>
export module andromeda:application;

import :utility;
import vulkan;
import std;

export namespace andromeda {
    class Application {
    public:
        void run() {
            std::cout << "Starting Andromeda application" << std::endl;
            initWindow();
            initVulkan();

            while (!glfwWindowShouldClose(window.get())) {
                glfwPollEvents();
                loop();
            }
            
        }

        virtual void loop() {};
    private:
        constexpr static int width {800};
        constexpr static int height {600};
        std::unique_ptr<GLFWwindow,void(*)(GLFWwindow*)> window{nullptr, {}};

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

    private:
        void initWindow();
        std::vector<const char*> getRequiredGLFWExtensions() const;

        // vulkan.cpp
        void initVulkan();
        void createInstance();

        // window.cpp
        void createSurface();

        // device.cpp
        bool isDeviceSuitable(const vk::raii::PhysicalDevice& physicalDevice);
        void selectPhysicalDevice();
        void createLogicalDevice();

        // swapchain.cpp
        void createSwapChain();
        void createImageViews();

        vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const;
        static std::uint32_t chooseSwapMinImageCount(const vk::SurfaceCapabilitiesKHR& capabilities);
        static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
        static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);


        // Debug only - vulkan.cpp
        void setupDebugMessages();
        static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
            vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, void*);
    };


}
