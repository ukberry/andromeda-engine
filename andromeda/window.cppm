module;
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
export module andromeda:window_glfw;

import vulkan;
import std.compat;
import :utility;

// void Application::initWindow() {
//         if (glfwInit() == GLFW_FALSE) {
//             std::cerr << "Cannot initialise the GLFW library!" << std::endl;
//             std::terminate(); 
//         }

//         glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//         glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // TODO: Support resizeable window
//         glfwWindowHint(GLFW_ICONIFIED, GLFW_TRUE);

//         window = std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)>{
//             glfwCreateWindow(width, height, "Application window", nullptr, nullptr),
//             [](GLFWwindow* win) { // Window deallocator
//                 if(!win) return;

//                 glfwDestroyWindow(win);

//                 // Once the window is destroyed, GLFW can be cleaned up.
//                 glfwTerminate();
//                 std::cout << "All resources deallocated!" << std::endl;
//             }
//         };
//     }

// std::vector<const char*> Application::getRequiredGLFWExtensions() const {
//     uint32_t glfwExtensionCount{};
//     auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

//     std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);        
//     return extensions;
// }

// void Application::createSurface() {
//     typedef struct VkSurfaceKHR_T *VkSurfaceKHR;
//     VkSurfaceKHR _surface;
//     if (glfwCreateWindowSurface(*instance, window.get(), nullptr, &_surface)) {
//         std::cerr << "Vulkan cannot initialise. Could not create a window surface.\n";
//         std::terminate();
//     }
//     surface = vk::raii::SurfaceKHR(instance, _surface);
// }

// std::pair<int,int> Application::getWindowSize() const {
//     std::pair<int,int> ret{};
//     glfwGetFramebufferSize(window.get(), &ret.first, &ret.second);
//     return ret;
// }

// bool Application::shouldWindowClose() const {
//     return glfwWindowShouldClose(window.get());
// }

// void Application::pollWindowEvents() {
//     glfwPollEvents();
// }
export namespace andromeda {
template <class Context>
class GLFWWindowPolicy {
public:
    bool CreateWindow() {
        std::cout << "GLFW CreateWindow called\n";
        return true;
    }
private:
    std::unique_ptr<GLFWwindow,void(*)(GLFWwindow*)> window{nullptr,{}};
};

template <class Context>
struct WindowTraits<GLFWWindowPolicy<Context>> {
    using type = GLFWwindow*;
};

} // namespace: andromeda