#pragma once

#include "config/scene_config.h"

// Entities
#include "entity/camera_game_object.h"
#include "entity/renderable_game_object.h"
#include "entity/utils/game_object_controller.h"

// Lights
#include "entity/lights/spot_light_game_object.h"

// Physics
#include "entity/physics/physics_game_object.h"
#include "entity/physics/particle_system/particle_system.h"
#include "entity/physics/spring.h"

#include "../input/mouse/Mouse.h"

namespace hrzn::scene
{
    static const std::string sc_defaultObjectModelFilepath = "res/models/";

    class SceneManager
    {
    public:
        SceneManager(entity::ControllerManager* controllerManager);
        ~SceneManager();

        bool initialise();

        bool loadScene(const char* sceneName);
        bool saveScene();
        bool saveSceneTGP();
        void unloadScene();

        void update(float deltaTime);
        void checkObjectCollisions(float deltaTime);

        entity::CameraGameObject& getCamera();

        const entity::GameObject& getGameObject(const std::string& label);
        entity::GameObject&       getWritableGameObject(const std::string& label);
        void                      removeGameObject(const std::string& gameObjectLabel);

        void mouseEventDelegate(const input::MouseEvent& mouseEvent, float deltaTime);
        
        void checkSelectingObject();
        void updateSelectedObject();

        void      floatObject(entity::GameObject* object);
        float     getWaterHeightAt(float posX, float posZ, bool exact = false);
        XMVECTOR  getFourierOffset(float x, float z);

    private:
        std::string m_sceneName;

        SceneConfig m_sceneConfig;

        entity::CameraGameObject*    m_activeCamera;

        entity::RenderableGameObject m_skybox;
        entity::RenderableGameObject m_clouds;
        entity::RenderableGameObject m_ocean;

        entity::LightGameObject      m_directionalLight;
        XMFLOAT3                     m_sunDirection;

        std::unordered_map<std::string, entity::GameObject*> m_gameObjectMap;
        entity::RenderableGameObject*                        m_selectedObject;

        std::vector<entity::RenderableGameObject*>       m_renderableGameObjects;
        std::vector<entity::physics::PhysicsGameObject*> m_physicsGameObjects;
        std::vector<entity::physics::Spring*>            m_springs;
        std::vector<entity::PointLightGameObject*>       m_pointLights;
        std::vector<entity::SpotLightGameObject*>        m_spotLights;
        std::vector<entity::CameraGameObject*>           m_cameras;

        entity::physics::ParticleSystem* m_particleSystem;

        entity::ControllerManager*                                m_controllerManager;
        std::unordered_map<std::string, entity::GameObjectTrack*> m_objectTracks;

        bool  m_dayNightCycle = false;
        bool  m_paused = false;
        float m_dayProgress = 0.494f;
        float m_gameTime = 0.0f;

        int         m_newObjectType;
        bool        m_newObjectMenuOpen = false;
        std::string m_newObjectLabel = "";
        std::string m_newObjectModelPath = "";
    };
}
