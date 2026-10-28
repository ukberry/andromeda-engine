export module andromeda:resources;

import :utility;

export namespace andromeda {

    // This separates the type IDs from other collections (ie Components)
    struct ResourceCategory {};

    // Forward declaration of Resource for template concept
    class Resource;

    template <class T>
    concept ResourceConcept = std::is_base_of_v<Resource, T> && !std::is_copy_constructible_v<T> && !std::is_copy_assignable_v<T>;
    
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
        explicit TestResource(resource_id id ) : Resource(id), resourceId(id) {
            std::cout << std::format("TestResource[{}] has been created\n",resourceId);
        }

        TestResource(const TestResource&) = delete;
        TestResource& operator=(const TestResource&) = delete;

        TestResource(TestResource&&) = default;
        TestResource& operator=(TestResource&&) = default;
        
        ~TestResource() {
            std::cout << std::format("TestResource[{}] has been destroyed\n",resourceId);
        }

        std::string TestMessage() const { return std::format("TestResource[{}]::TestMessage called. The type id for this class is {}",
            resourceId,TypeRegistry::getTypeID<ResourceCategory, TestResource>()); }
    protected:
        bool doLoad() { 
            std::cout << std::format("TestResource[{}] has been loaded\n", resourceId);
            return true;
        }
        void doUnload() {
            std::cout << std::format("TestResource[{}] has been unloaded\n", resourceId);
        }
    private:
        resource_id resourceId{};
    };

    template <class T>
    class ResourceHandle;

    class ResourceManager {
    public:
        template <ResourceConcept T, typename... Args>
        requires std::is_constructible_v<T, resource_id, Args...>
        ResourceHandle<T> Load(std::string_view id, Args&&... args) {
            return Load<T>( FNVHash::hash(id), std::forward<Args>(args)...);
        }

        template <ResourceConcept T, typename... Args>
        requires std::is_constructible_v<T, resource_id, Args...>
        ResourceHandle<T> Load(resource_id id, Args&&... args) {

            auto type_id {TypeRegistry::getTypeID<ResourceCategory,T>()};
            auto& scoped_resources { resources[type_id] };
            auto& scoped_reference_counts { refCounts[type_id] };
            auto it = scoped_resources.find(id);

            if (it != scoped_resources.end()) {
                scoped_reference_counts[id].refCount++;
                return ResourceHandle<T>(id, this);
            }

            auto resource { std::make_shared<T>(id, std::forward<Args>(args)...) };
            if(!resource->Load()) {
                return {};
            }

            scoped_resources[id] = resource;
            scoped_reference_counts[id].resource = resource;
            scoped_reference_counts[id].refCount = 1;
            return ResourceHandle<T>(id, this);
        }

        template <ResourceConcept T>
        T* GetResource(resource_id id) {
            auto type_id {TypeRegistry::getTypeID<ResourceCategory, T>()};

            auto& scoped_resources { resources[type_id] };
            auto it = scoped_resources.find(id);

            if (it != scoped_resources.end()) return static_cast<T*>(it->second.get());
            return nullptr;
        }

        template <ResourceConcept T>
        constexpr T* GetResource(std::string_view id) {
            return GetResource<T>( FNVHash::hash(id) );
        }

        template <ResourceConcept T>
        bool HasResource(const std::string_view resourceId) {
            const auto& scoped_resource { resources[TypeRegistry::getTypeID<ResourceCategory,T>()] };
            const auto it { scoped_resource.find(FNVHash::hash(resourceId)) };
            return it != resources.end();
        }

        template <ResourceConcept T>
        void Release(resource_id id) {
            
            const auto type_id {TypeRegistry::getTypeID<ResourceCategory,T>()};
            auto& scoped_resource { resources[type_id] };
            auto& scoped_references { refCounts[type_id] };
            
            auto it = scoped_references.find(id);
            if (it != scoped_references.end()) {
                it->second.refCount--;

                if(it->second.refCount > 0) return;
                auto resourceIt {scoped_resource.find(id)};

                resourceIt->second->Unload();
                scoped_resource.erase(resourceIt);
                scoped_references.erase(it);
            }
        }

        template <ResourceConcept T>
        constexpr void Release(std::string_view id) {
            return Release<T>( FNVHash::hash(id) );
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