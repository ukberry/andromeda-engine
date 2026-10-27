export module andromeda:resources;

import :utility;

export namespace andromeda {

    // This separates the type IDs from other collections (ie Components)
    struct ResourceCategory {};
    
    /** @brief A base class for resource management through the ResourceManager class.
     */
    class Resource {
    public:
        explicit Resource(resource_id id) : resourceId(id) {}
        explicit Resource(const std::string_view id) : resourceId( FNVHash::hash(id) ) {}
        virtual ~Resource() = default;

        resource_id GetId() const{ return resourceId; }
        bool IsLoaded() const { return loaded; }

        bool Load() {
            loaded = doLoad();
            return loaded;
        }

        void Unload() {
            doUnload();
            loaded = false;
        }
    protected:
        virtual bool doLoad() = 0;
        virtual void doUnload() = 0;
    private:
        resource_id resourceId{};
        bool loaded { false };
    };

    class TestResource final : public Resource {
    public:
        explicit TestResource(resource_id id ) : Resource(id) {}
        std::string TestMessage() const { return std::format("This is a test resource message. The type id for this class is {}",
            TypeRegistry::getTypeID<ResourceCategory, TestResource>()); }
    protected:
        bool doLoad() { return true; }
        void doUnload() {}
    };

    template <class T>
    class ResourceHandle;

    class ResourceManager {
    public:
        template <typename T>
        ResourceHandle<T> Load(std::string_view id) {
            return Load<T>( FNVHash::hash(id) );
        }

        template <typename T>
        ResourceHandle<T> Load(resource_id id) {
            static_assert(std::is_base_of<Resource, T>::value, "Load: Type must be derived from Resource!");

            auto type_id {TypeRegistry::getTypeID<ResourceCategory,T>()};
            auto& scoped_resources { resources[type_id] };
            auto& scoped_reference_counts { refCounts[type_id] };
            auto it = scoped_resources.find(id);

            if (it != scoped_resources.end()) {
                scoped_reference_counts[id].refCount++;
                return ResourceHandle<T>(id, this);
            }

            auto resource { std::make_shared<T>(id) };
            if(!resource->Load()) {
                return {};
            }

            scoped_resources[id] = resource;
            scoped_reference_counts[id].resource = resource;
            scoped_reference_counts[id].refCount = 1;
            return ResourceHandle<T>(id, this);
        }

        template <typename T>
        T* GetResource(resource_id id) {
            static_assert(std::is_base_of<Resource, T>::value, "GetResource: Type must be derived from Resource!");
            auto type_id {TypeRegistry::getTypeID<ResourceCategory, T>()};

            auto& scoped_resources { resources[type_id] };
            auto it = scoped_resources.find(id);

            if (it != scoped_resources.end()) return static_cast<T*>(it->second.get());
            return nullptr;
        }

        template <typename T>
        T* GetResource(std::string_view id) {
            return GetResource<T>( FNVHash::hash(id) );
        }

        template<typename T>
        bool HasResource(const std::string_view resourceId) {
            static_assert(std::is_base_of<Resource, T>::value, "HasResource: Type must be derived from Resource!");
            const auto& scoped_resource { resources[TypeRegistry::getTypeID<ResourceCategory,T>()] };
            const auto it { scoped_resource.find(FNVHash::hash(resourceId)) };
            return it != resources.end();
        }

    private:
        struct ResourceData {
            int refCount{};
            std::shared_ptr<Resource> resource;
        };
        std::unordered_map<int, std::unordered_map<resource_id, std::shared_ptr<Resource>>> resources;
        std::unordered_map<int, std::unordered_map<resource_id, ResourceData>> refCounts;

    };

    template <typename T>
    class ResourceHandle {
    private:
        resource_id resourceId;
        ResourceManager *resourceManager;
    public:
        ResourceHandle() : resourceManager{nullptr} {}
        ResourceHandle(const std::string_view id, ResourceManager *manager) :
            resourceId{FNVHash::hash(id)}, resourceManager{manager} {};
        ResourceHandle(resource_id id, ResourceManager *manager) : resourceId{id}, resourceManager{manager} {};

        T* Get() const {
            if (!resourceManager) return nullptr;
            return resourceManager->GetResource<T>(resourceId);
        }
    };

    struct Component {
    public:
        virtual ~Component() = default;
        virtual void Update(float) {}
    };

    class MeshComponent final : public Component {};

} // namespace: andromeda