#pragma once

#include "config/scene_config.h"

// Scene
#include "scene_loader.h"

// Entities
#include "../entity/camera_game_object.h"
#include "../entity/renderable_game_object.h"
#include "../entity/utils/game_object_controller.h"
#include "../entity/physics_game_object.h"

// Lights
#include "../entity/lights/spot_light_game_object.h"

// Physics
#include "../physics/particle_system/particle_system.h"
#include "../physics/spring.h"

#include "../input/mouse/Mouse.h"

namespace hrzn::scene
{
    static const std::string sc_defaultObjectModelFilepath = "res/models/";

    enum class AxisEditState
    {
        eEditNone,
        eEditTranslate,
        eEditRotate,
        eEditScale,
    };

    enum class AxisEditSubState
    {
        eEditX,
        eEditY,
        eEditZ,
        eEditNone,
    };

    class SceneManager
    {
    public:
        SceneManager(entity::ControllerManager* controllerManager);
        ~SceneManager();

        bool initialise();

        const std::string& getSceneName();

        // Load/save/unload funcs
        bool loadScene(const char* sceneName);
        bool loadSceneStaticObjects();
        bool saveScene(const char* sceneName);
        void unloadScene();

        // Updating
        void update(float deltaTime);
        void checkObjectCollisions(float deltaTime);

        // All objects
        const std::unordered_map<std::string, entity::GameObject*>& getObjectMap() const;
        std::unordered_map<std::string, entity::GameObject*>&       getWritableObjectMap();

        // Camera(s)
        const entity::CameraGameObject&               getActiveCamera() const;
        void                                          setActiveCamera(entity::CameraGameObject* camera);

        entity::CameraGameObject&                     getWritableActiveCamera();
        const std::vector<entity::CameraGameObject*>& getCameraList() const;
        std::vector<entity::CameraGameObject*>&       getWritableCameraList();

        // Lights
        const entity::LightGameObject& getDirectionalLight() const;
        entity::LightGameObject&       getWritableDirectionalLight();

        const std::vector<entity::PointLightGameObject*>& getPointLights() const;
        std::vector<entity::PointLightGameObject*>&       getWritablePointLights();

        const std::vector<entity::SpotLightGameObject*>& getSpotLights() const;
        std::vector<entity::SpotLightGameObject*>&       getWritableSpotLights();

        // Renderable
        const std::vector<entity::RenderableGameObject*>& getRenderables() const;
        std::vector<entity::RenderableGameObject*>&       getWritableRenderables();

        const entity::RenderableGameObject& getSkybox() const;
        entity::RenderableGameObject&       getWritableSkybox();

        const entity::RenderableGameObject& getOcean() const;
        entity::RenderableGameObject&       getWritableOcean();

        const entity::RenderableGameObject& getClouds() const;
        entity::RenderableGameObject&       getWritableClouds();

        // Physics
        const std::vector<entity::PhysicsGameObject*>& getPhysicsObjects() const;
        std::vector<entity::PhysicsGameObject*>&       getWritablePhysicsObjects();

        const std::vector<physics::Spring*>& getSprings() const;
        std::vector<physics::Spring*>&       getWritableSprings();

        const physics::ParticleSystem& getParticleSystem() const;
        physics::ParticleSystem&       getWritableParticleSystem();

        // Generic
        const entity::GameObject& getGameObject(const std::string& label) const;
        entity::GameObject&       getWritableGameObject(const std::string& label);

        void                      addGameObject(entity::GameObject* gameObject);
        void                      removeGameObject(const std::string& gameObjectLabel);

        void                      addObjectTrack(entity::GameObjectTrack* objectTrack);
        std::unordered_map<std::string, entity::GameObjectTrack*>& getObjectTracks();

        const SceneConfig&        getSceneConfig() const;
        SceneConfig&              getWritableSceneConfig();

        void mouseButtonDelegate(const input::MouseEvent& mouseEvent, float deltaTime);
        void mouseScrollDelegate(const input::MouseEvent& mouseEvent, float deltaTime);
        void mouseMoveDelegate(const input::MouseEvent& mouseEvent, float deltaTime);

        void                                checkSelectingObject();
        void                                updateSelectedObject();
        bool                                objectIsSelected() const;
        const entity::RenderableGameObject* getSelectedObject() const;
        entity::RenderableGameObject*       getWritableSelectedObject();
        void                                setSelectedObject(entity::RenderableGameObject* object);

        void      floatObject(entity::GameObject* object) const;
        float     getWaterHeightAt(float posX, float posZ, bool exact = false) const;
        XMVECTOR  getFourierOffset(float x, float z) const;

        float  getGameTime() const;
        float  getDayProgress() const;
        float* getDayProgressPtr();
        bool   isUsingDayNightCycle() const;
        bool*  getDayNightCyclePtr();
        bool   isPaused() const;
        bool*  getPausedPtr();

        const XMVECTOR&  getMostAllignedVector(const XMVECTOR& testVec1, const XMVECTOR& testVec2, const XMVECTOR& dir);

        AxisEditState    getAxisEditState() const;
        void             setAxisEditState(AxisEditState axisEditState);

        AxisEditSubState getAxisEditSubState() const;
        void             setAxisEditSubState(AxisEditSubState axisEditSubState);

        void             stopAxisEdit();

        entity::ControllerManager* getWritableControllerManager();

    private:
        std::string m_sceneName;

        SceneLoader m_sceneLoader;

        SceneConfig m_sceneConfig;

        entity::CameraGameObject*    m_activeCamera;

        entity::RenderableGameObject m_skybox;
        entity::RenderableGameObject m_clouds;
        entity::RenderableGameObject m_ocean;

        // Object map and lists
        std::unordered_map<std::string, entity::GameObject*> m_gameObjectMap;
        entity::GameObject                                   m_badReturnGameObject;

        std::vector<entity::CameraGameObject*>     m_cameras;
        std::vector<entity::RenderableGameObject*> m_renderables;
        std::vector<entity::PhysicsGameObject*>    m_physicsObjects;
        std::vector<physics::Spring*>              m_springs;

        entity::LightGameObject                    m_directionalLight;
        std::vector<entity::PointLightGameObject*> m_pointLights;
        std::vector<entity::SpotLightGameObject*>  m_spotLights;

        // Object selection and modification
        entity::RenderableGameObject* m_selectedObject;

        XMFLOAT3    m_axisTranslateDefaultBounds[3];
        BoundingBox m_axisTranslateBoudingBoxes[3];

        AxisEditState    m_axisEditState = AxisEditState::eEditTranslate;
        AxisEditSubState m_axisEditSubState = AxisEditSubState::eEditNone;

        float    m_lastAxisGrabOffset = FLT_MAX;
        XMVECTOR m_lastGrabPos;
        XMVECTOR m_axisEditPlaneNormal;

        physics::ParticleSystem* m_particleSystem;

        entity::ControllerManager*                                m_controllerManager;
        std::unordered_map<std::string, entity::GameObjectTrack*> m_objectTracks;

        bool  m_dayNightCycle;
        bool  m_paused;
        float m_dayProgress;
        float m_gameTime;

        int         m_newObjectType;
        bool        m_newObjectMenuOpen;
        std::string m_newObjectLabel;
        std::string m_newObjectModelPath;
    };
}
