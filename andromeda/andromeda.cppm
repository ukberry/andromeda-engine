module;
#include <GLFW/glfw3.h>
#include <vulkan/vk_platform.h>
export module andromeda;

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

        vk::raii::Context context;
        vk::raii::Instance instance {nullptr};
        vk::raii::DebugUtilsMessengerEXT debugMessenger {nullptr};
        vk::raii::SurfaceKHR surface {nullptr};
        vk::raii::PhysicalDevice physicalDevice {nullptr};

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


        // Debug only - vulkan.cpp
        void setupDebugMessages();
        static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
            vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, void*);
    };
}