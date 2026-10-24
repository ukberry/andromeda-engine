module;
#include <GLFW/glfw3.h>
module andromeda;

namespace andromeda {
    void Application::initWindow() {
            if (glfwInit() == GLFW_FALSE) throw std::runtime_error{"Cannot initialise the GLFW library"};

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
}