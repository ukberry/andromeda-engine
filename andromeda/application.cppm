export module andromeda:application;

export import :window_glfw;
export import :utility;
import vulkan;
import std;

export namespace andromeda {
    
    // CRTP template on all policies passed
    template <template <typename> class... Policies>
    class Context : public Policies<Context<Policies...>>... {
    public:
        using WindowPolicy = typename GetWindowPolicy<Context, Policies...>::type;
        //using WindowType = typename WindowTraits<WindowPolicy>::type;
        using WindowType = typename GetWindowPolicy<Context, Policies...>::window_type;
        bool Initialise() {

            bool windowInitialised{};
            if constexpr (requires { {this->CreateWindow()} -> std::same_as<bool>; } ) {
                windowInitialised = this->CreateWindow();
            }
            return windowInitialised;
        }
        
        bool IsClosing() {
            if constexpr (requires { {this->ShouldWindowClose()} -> std::same_as<bool>; }) {
                return this->ShouldWindowClose();
            }
            return true;
        }

        void PollEvents() {
            return this->PollWindowEvents();
        }
    public:
        std::unique_ptr<WindowType,void(*)(WindowType*)> window{nullptr,{}};
    };

    template <class Context>
    class Application {
    public:
        void run() {
            std::cout << "Starting Andromeda application" << std::endl;
            context.Initialise();
            // initWindow();
            // initVulkan();

            while (!context.IsClosing()) {
                context.PollEvents();
                loop();
            }
            
        }

        virtual void loop() {};
    private:
        constexpr static int width {800};
        constexpr static int height {600};
        Context context{};
    //     std::unique_ptr<WindowType,void(*)(WindowType*)> window{nullptr, {}};

    //     // Vulkan setup
    //     vk::raii::Context context;
    //     vk::raii::Instance instance {nullptr};
    //     vk::raii::DebugUtilsMessengerEXT debugMessenger {nullptr};

    //     // Window surface setup
    //     vk::raii::SurfaceKHR surface {nullptr};

    //     // Device setup
    //     vk::raii::PhysicalDevice physicalDevice {nullptr};
    //     vk::raii::Device device {nullptr};
    //     uint32_t queueIndex {~0u};
    //     vk::raii::Queue queue {nullptr};

    //     // Swapchain setup
    //     vk::raii::SwapchainKHR swapChain {nullptr};
    //     vk::Extent2D swapChainExtent;
    //     vk::SurfaceFormatKHR swapChainSurfaceFormat;
    //     std::vector<vk::Image> swapChainImages;
    //     std::vector<vk::raii::ImageView> swapChainImageViews;

    //     std::vector<const char*> requiredDeviceExtensions {
    //         vk::KHRSwapchainExtensionName
    //     };

    // private:
    //     void initWindow();
    //     std::vector<const char*> getRequiredGLFWExtensions() const;

    //     // vulkan.cpp
    //     void initVulkan();
    //     void createInstance();

    //     // window.cpp
    //     void createSurface();
    //     std::pair<int,int> getWindowSize() const;
    //     bool shouldWindowClose() const;
    //     void pollWindowEvents();

    //     // device.cpp
    //     bool isDeviceSuitable(const vk::raii::PhysicalDevice& physicalDevice);
    //     void selectPhysicalDevice();
    //     void createLogicalDevice();

    //     // swapchain.cpp
    //     void createSwapChain();
    //     void createImageViews();

    //     vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const;
    //     static std::uint32_t chooseSwapMinImageCount(const vk::SurfaceCapabilitiesKHR& capabilities);
    //     static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
    //     static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);


    //     // Debug only - vulkan.cpp
    //     void setupDebugMessages();
    //     static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
    //         vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, void*);
    };


}
