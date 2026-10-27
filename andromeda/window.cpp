module;
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
module andromeda;

import vulkan;
import std.compat;

using namespace andromeda;

void Application::initWindow() {
        if (glfwInit() == GLFW_FALSE) {
            std::cerr << "Cannot initialise the GLFW library!" << std::endl;
            std::terminate(); 
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // TODO: Support resizeable window
        glfwWindowHint(GLFW_ICONIFIED, GLFW_TRUE);

        window = std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)>{
            glfwCreateWindow(width, height, "Application window", nullptr, nullptr),
            [](GLFWwindow* win) { // Window deallocator
                if(!win) return;

                glfwDestroyWindow(win);

                // Once the window is destroyed, GLFW can be cleaned up.
                glfwTerminate();
                std::cout << "All resources deallocated!" << std::endl;
            }
        };
    }

std::vector<const char*> Application::getRequiredGLFWExtensions() const {
    uint32_t glfwExtensionCount{};
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);        
    return extensions;
}

void Application::createSurface() {
    typedef struct VkSurfaceKHR_T *VkSurfaceKHR;
    VkSurfaceKHR _surface;
    if (glfwCreateWindowSurface(*instance, window.get(), nullptr, &_surface)) {
        std::cerr << "Vulkan cannot initialise. Could not create a window surface.\n";
        std::terminate();
    }
    surface = vk::raii::SurfaceKHR(instance, _surface);
}
