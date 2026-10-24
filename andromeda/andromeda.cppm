module;
#include <GLFW/glfw3.h>
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

    private:
        void initWindow();

        void initVulkan();
        void createInstance();
    };
}