export module andromeda:components;

import :utility;

namespace andromeda {

    struct ComponentCategory {};

    class Entity;

    class Component {
    public:
        Entity* GetOwner() const { return owner; }

        template <class T>
        static int GetTypeID() {
            return TypeRegistry::getTypeID<ComponentCategory,T>();
        }
    private:
        Entity *owner {nullptr};
    };

    class Entity {
    public:
        template<class T, typename... Args>
        T* AddComponent(Args&& ...args) {
            static_assert(std::is_base_of_v<Component, T>, "Component class must derive from andromeda::Component");

            constexpr int typeID {Component::GetTypeID<T>()};

            auto it {componentMap.find(typeID)};
            if (it != componentMap.end()) {
                return static_cast<T*>(it->second);
            }

            auto component {std::make_unique<T>(std::forward<Args>(args)...)};
            T* componentPtr {component.get()};
            componentMap[typeID] = componentPtr;
            components.push_back(std::move(component));
            return componentPtr;
        }

        template <class T>
        T* GetComponent() {
            constexpr int typeID {Component::GetTypeID<T>()};
            auto it {componentMap.find(typeID)};
            
            if (it == componentMap.end()) return nullptr;

            return static_cast<T*>(it->second);
        }

        template <class T>
        bool RemoveComponent() {
            constexpr int typeID {Component::GetTypeID<T>()};
            auto it {componentMap.find(typeID)};

            if (it == componentMap.end()) return false;

            T* componentPtr {it->second};
            componentMap.erase(it);

            return std::erase_if(components, [componentPtr](const auto& component) {
                return component.get() == componentPtr;
            }) > 0;

        }
    private:
        std::vector<std::unique_ptr<Component>> components;
        std::unordered_map<int, Component*> componentMap;
    };

}
