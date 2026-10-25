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

    private:
        void initWindow();
        std::vector<const char*> getRequiredGLFWExtensions() const;

        void initVulkan();
        void createInstance();
        void createSurface();

        // Debug only
        void setupDebugMessages();
        static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
            vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, void*);
    };
}