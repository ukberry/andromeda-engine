export module andromeda;

export import :utility;
export import :application;
import std;

export namespace andromeda {


    class Resource;
    class ResourceManager;
    template <typename T>
    class ResourceHandle;

    struct Component {
    public:
        virtual ~Component() = default;
        virtual void Update(float) {}
    };

    class MeshComponent final : public Component {};

    
    

    struct ResourceCategory {};
} // namespace: andromeda

