export module andromeda:culling_system;

import :components;

import std;

namespace andromeda {

    class Scene {
    public:
        std::vector<Entity*> GetEntities() const;
    private:
        std::vector<std::unique_ptr<Entity>> entities;
    };

    class CullingSystem {
    public:
        explicit CullingSystem(Scene * scene) : scene(scene) {}

        void CullScene([[maybe_unused]]std::uint16_t cameraIndex = 0) {
            
            visibleEntities.clear();
            // GetCurrentCamera
            auto entities {scene->GetEntities()};
            std::ranges::copy_if(entities, std::back_inserter(visibleEntities), []([[maybe_unused]]const Entity* entity) {
                return true;
            });

        } 

        const std::vector<Entity*>& GetVisibleEntities() const {
            return visibleEntities;
        }
    private:
        Scene* scene;
        std::vector<Entity*> visibleEntities;
    };

} // namespace: andromeda
