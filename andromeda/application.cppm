export module andromeda:application;

export import :window_glfw;
export import :utility;
export import :graphics_vulkan;
import vulkan;
import std;

export namespace andromeda {
    
    // CRTP template on all policies passed
    template <template <typename> class... Policies>
    class Context : public Policies<Context<Policies...>>... {
    public:
        using WindowPolicy = typename GetWindowPolicy<Context, Policies...>::type;
        
        /// Two ways to obtain the window type; the window_type on the policy is most convenient
        //using WindowType = typename WindowTraits<WindowPolicy>::type;
        using WindowType = typename GetWindowPolicy<Context, Policies...>::window_type;
        bool Initialise() {

            bool windowInitialised{};
            if constexpr (requires { {this->CreateWindow()} -> std::same_as<bool>; } ) {
                windowInitialised = this->CreateWindow();
            }

            bool graphicsInitialised{};
            if constexpr (requires { {this->InitGraphics()} -> std::same_as<bool>; }) {
                graphicsInitialised = this->InitGraphics();
            }
            if (graphicsInitialised) std::cout << "Vulkan graphics surface was created!\n";
            return windowInitialised && graphicsInitialised;
        }
    };

    template <class Context>
    class Application {
    public:
        void Run() {
            std::cout << "Starting Andromeda application" << std::endl;
            if(!context.Initialise()) {
                std::cerr << "Could not initialise the game engine context" << std::endl;
                return;
            }
            // initWindow();
            // initVulkan();

            while (!context.ShouldWindowClose()) {
                context.PollWindowEvents();
                Loop();
            }
            
        }

        virtual void Loop() {};
    private:
        constexpr static int width {800};
        constexpr static int height {600};
        Context context{};
    };


}
