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

export constexpr andromeda::resource_id operator""_fnv(const char* str, std::size_t size) noexcept {
    return andromeda::FNVHash::hash(std::string_view(str,size));
}
