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
    private:
        std::vector<std::unique_ptr<Component>> components;
    };



}
