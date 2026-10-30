module;
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
export module andromeda:window_glfw;

import vulkan;
import std.compat;
import :utility;

export namespace andromeda {


template <class T>
concept WindowPolicy = requires(T policy) {
    {std::same_as<typename T::is_window_policy,void>};
};

template <class Context>
class GLFWWindowPolicy {
public:
    using is_window_policy = void;
    using WindowType = GLFWwindow;
    bool CreateWindow() {
        std::cout << "GLFW CreateWindow called\n";
        if (!glfwInit()) {
            std::cerr << "Could not load GLFW\n";
            std::terminate();
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // TODO: Support resizeable window
        glfwWindowHint(GLFW_ICONIFIED, GLFW_TRUE);

        window = std::unique_ptr<GLFWwindow,void(*)(GLFWwindow* win)>
        {glfwCreateWindow(800,600,"Test",nullptr,nullptr),
        [](GLFWwindow* win) {
            if(!win) return;
            glfwDestroyWindow(win);
            glfwTerminate();
        }};

        return true;
    }

    bool ShouldWindowClose() {
        auto crtp_self {static_cast<Context*>(this)};
        if(!crtp_self->window.get()) return true;
        return glfwWindowShouldClose(crtp_self->window.get());
    }

    void PollWindowEvents() {
        glfwPollEvents();
    }

    std::pair<int,int> GetWindowSize() const {
        std::pair<int,int> ret{};
        glfwGetFramebufferSize(window.get(), &ret.first, &ret.second);
        return ret;
    }

    WindowType* GetWindow() const {
        return window.get();
    }

    vk::raii::SurfaceKHR CreateSurface() {
    typedef struct VkSurfaceKHR_T *VkSurfaceKHR;
    VkSurfaceKHR _surface;
    if (glfwCreateWindowSurface(*static_cast<Context*>(this)->GetVulkanInstance(), window.get(), nullptr, &_surface)) {
        std::cerr << "Vulkan cannot initialise. Could not create a window surface.\n";
        std::terminate();
    }
    return vk::raii::SurfaceKHR(static_cast<Context*>(this)->GetVulkanInstance(), _surface);
}

    std::vector<const char*> GetRequiredWindowExtensions() const {
    uint32_t glfwExtensionCount{};
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);        
    return extensions;
}
protected:
    std::unique_ptr<GLFWwindow,void(*)(GLFWwindow*)> window{nullptr,{}};
};

template <class Context>
struct WindowTraits<GLFWWindowPolicy<Context>> {
    using type = GLFWwindow;
};

} // namespace: andromeda