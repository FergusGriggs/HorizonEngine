#include "scene_loader.h"

#include <fstream>
#include <sstream>
#include <algorithm>

#include "../utils/error_logger.h"
#include "../user_config.h"

#include "scene_manager.h"

namespace hrzn::scene
{
    SceneLoader::SceneLoader(SceneManager* sceneManager) :
        m_sceneManager(sceneManager)
    {
    }

    SceneLoader::~SceneLoader()
    {
    }

    void SceneLoader::combineResults(const LoadResult& compare, LoadResult& storage)
    {
        if (compare > storage) storage = compare;
    }

    SceneLoader::LoadResult SceneLoader::loadScene(std::string sceneName)
    {
        std::string sceneFilePath = "res/scenes/";
        sceneFilePath += sceneName + ".hrzn";

        std::ifstream sceneFile(sceneFilePath.c_str());

        if (!sceneFile) return LoadResult::eFailed;

        std::stringstream buffer;
        buffer << sceneFile.rdbuf();
        std::string sceneFileStr = buffer.str();

        // Parse the data loaded from file using rapidjson
        rapidjson::Document document;
        document.Parse(sceneFileStr.c_str());
        if (document.HasParseError())
        {
            rapidjson::ParseErrorCode error = document.GetParseError();
            size_t errorOffset = document.GetErrorOffset();

            int errorStart = std::max(0, static_cast<int>(errorOffset) - 20);
            int errorEnd = std::min(strlen(sceneFileStr.c_str()), errorOffset + 20);

            int beforeLen = errorOffset - errorStart;
            int afterLen = errorEnd - errorOffset;

            std::string errorString = sceneFileStr.substr(errorStart, beforeLen) + "[HERE]" + sceneFileStr.substr(errorOffset, afterLen);
            errorString.erase(std::remove(errorString.begin(), errorString.end(), '\n'), errorString.end());
            errorString.erase(std::remove(errorString.begin(), errorString.end(), '\t'), errorString.end());

            int lineNumber = utils::string_helpers::getLineNumberFromOffset(sceneFileStr, errorOffset);

            std::string finalErrorString = "Parsing Scene JSON\nCode: '" + std::to_string(error) + "'\nLine Num: '" + std::to_string(lineNumber) + "'\nError String: '" + errorString + "'\n";

            utils::ErrorLogger::log(finalErrorString);
            return LoadResult::eFailed;
        }

        // Ensure it has a scene object
        if (!document.HasMember("Scene"))
        {
            utils::ErrorLogger::log("Parsing Scene JSON\nNo scene tag found");
            return LoadResult::eFailed;
        }
        rapidjson::Value& sceneObject = document["Scene"];


        LoadResult loadResult = LoadResult::eOk;

        combineResults(loadMeta(sceneObject), loadResult);

        combineResults(loadConfigs(sceneObject), loadResult);

        combineResults(loadObjectTracks(sceneObject), loadResult);

        combineResults(loadGameObjects(sceneObject), loadResult);

        return loadResult;
    }

    SceneLoader::LoadResult SceneLoader::loadMeta(rapidjson::Value& sceneObject)
    {
        return LoadResult::eOk;
    }

    SceneLoader::LoadResult SceneLoader::loadConfigs(rapidjson::Value& sceneObject)
    {
        if (!sceneObject.HasMember("SceneConfig"))
        {
            utils::ErrorLogger::log("Parsing Scene JSON\nNo 'SceneConfig' tag found");
            return LoadResult::eFailed;
        }
        rapidjson::Value& sceneConfig = sceneObject["SceneConfig"];

        LoadResult result = LoadResult::eOk;

        combineResults(loadTimeConfig(sceneConfig), result);
        combineResults(loadOceanConfig(sceneConfig), result);
        combineResults(loadCloudConfig(sceneConfig), result);
        combineResults(loadAtmosphereConfig(sceneConfig), result);

        return result;
    }

    SceneLoader::LoadResult SceneLoader::loadTimeConfig(rapidjson::Value& sceneConfig)
    {
        if (!sceneConfig.HasMember("TimeConfig"))
        {
            utils::ErrorLogger::log("Parsing Scene JSON\nNo 'TimeConfig' tag found");
            return LoadResult::eFailed;
        }
        rapidjson::Value& jsonTimeConfig = sceneConfig["TimeConfig"];

        config::TimeConfig timeConfig;

        if (jsonTimeConfig.HasMember("start_time")) timeConfig.m_startTime = jsonTimeConfig["start_time"].GetFloat();
        if (jsonTimeConfig.HasMember("day_night_cycle")) timeConfig.m_dayNightCycle = jsonTimeConfig["day_night_cycle"].GetBool();
        if (jsonTimeConfig.HasMember("time_paused")) timeConfig.m_timePaused = jsonTimeConfig["time_paused"].GetBool();
        if (jsonTimeConfig.HasMember("day_duration")) timeConfig.m_dayDuration = jsonTimeConfig["day_duration"].GetFloat();

        m_sceneManager->getWritableSceneConfig().setTimeConfig(timeConfig);

        return LoadResult::eOk;
    }

    SceneLoader::LoadResult SceneLoader::loadOceanConfig(rapidjson::Value& sceneConfig)
    {
        if (!sceneConfig.HasMember("OceanConfig"))
        {
            utils::ErrorLogger::log("Parsing Scene JSON\nNo 'OceanConfig' tag found");
            return LoadResult::eFailed;
        }
        rapidjson::Value& jsonOceanConfig = sceneConfig["OceanConfig"];

        config::OceanConfig oceanConfig;

        if (jsonOceanConfig.HasMember("enabled")) oceanConfig.m_enabled = jsonOceanConfig["enabled"].GetBool();

        if (jsonOceanConfig.HasMember("wave_count")) oceanConfig.m_waveCount = jsonOceanConfig["wave_count"].GetInt();

        if (jsonOceanConfig.HasMember("wave_scale")) oceanConfig.m_waveScale = jsonOceanConfig["wave_scale"].GetFloat();
        if (jsonOceanConfig.HasMember("wave_scale_multiplier")) oceanConfig.m_waveScaleMultiplier = jsonOceanConfig["wave_scale_multiplier"].GetFloat();

        if (jsonOceanConfig.HasMember("wave_period")) oceanConfig.m_wavePeriod = jsonOceanConfig["wave_period"].GetFloat();
        if (jsonOceanConfig.HasMember("wave_speed")) oceanConfig.m_waveSpeed = jsonOceanConfig["wave_speed"].GetFloat();
        if (jsonOceanConfig.HasMember("wave_seed")) oceanConfig.m_waveSeed = jsonOceanConfig["wave_seed"].GetFloat();

        if (jsonOceanConfig.HasMember("foam_start")) oceanConfig.m_foamStart = jsonOceanConfig["foam_start"].GetFloat();
        if (jsonOceanConfig.HasMember("colour_change_start")) oceanConfig.m_colourChangeStart = jsonOceanConfig["colour_change_start"].GetFloat();

        m_sceneManager->getWritableSceneConfig().setOceanConfig(oceanConfig);

        return LoadResult::eOk;
    }

    SceneLoader::LoadResult SceneLoader::loadCloudConfig(rapidjson::Value& sceneConfig)
    {
        if (!sceneConfig.HasMember("CloudConfig"))
        {
            utils::ErrorLogger::log("Parsing Scene JSON\nNo 'CloudConfig' tag found");
            return LoadResult::eFailed;
        }
        rapidjson::Value& jsonCloudConfig = sceneConfig["CloudConfig"];

        config::CloudConfig cloudConfig;

        if (jsonCloudConfig.HasMember("enabled")) cloudConfig.m_enabled = jsonCloudConfig["enabled"].GetBool();

        if (jsonCloudConfig.HasMember("light_absorbtion_through_clouds")) cloudConfig.m_lightAbsorbtionThroughClouds = jsonCloudConfig["light_absorbtion_through_clouds"].GetFloat();
        if (jsonCloudConfig.HasMember("light_absorbtion_towards_sun")) cloudConfig.m_lightAbsorbtionTowardsSun = jsonCloudConfig["light_absorbtion_towards_sun"].GetFloat();
        if (jsonCloudConfig.HasMember("phase_factor")) cloudConfig.m_phaseFactor = jsonCloudConfig["phase_factor"].GetFloat();
        if (jsonCloudConfig.HasMember("darkness_threshold")) cloudConfig.m_darknessThreshold = jsonCloudConfig["darkness_threshold"].GetFloat();

        if (jsonCloudConfig.HasMember("cloud_speed")) cloudConfig.m_cloudSpeed = jsonCloudConfig["cloud_speed"].GetFloat();
        if (jsonCloudConfig.HasMember("cloud_coverage")) cloudConfig.m_cloudCoverage = jsonCloudConfig["cloud_coverage"].GetFloat();
        if (jsonCloudConfig.HasMember("cloud_height")) cloudConfig.m_cloudHeight = jsonCloudConfig["cloud_height"].GetFloat();

        if (jsonCloudConfig.HasMember("num_steps")) cloudConfig.m_numSteps = jsonCloudConfig["num_steps"].GetInt();
        if (jsonCloudConfig.HasMember("step_size")) cloudConfig.m_stepSize = jsonCloudConfig["step_size"].GetFloat();

        m_sceneManager->getWritableSceneConfig().setCloudConfig(cloudConfig);

        return LoadResult::eOk;
    }

    SceneLoader::LoadResult SceneLoader::loadAtmosphereConfig(rapidjson::Value& sceneConfig)
    {
        if (!sceneConfig.HasMember("AtmosphereConfig"))
        {
            utils::ErrorLogger::log("Parsing Scene JSON\nNo 'AtmosphereConfig' tag found");
            return LoadResult::eFailed;
        }
        rapidjson::Value& jsonAtmosphereConfig = sceneConfig["AtmosphereConfig"];

        config::AtmosphereConfig atmosphereConfig;

        if (jsonAtmosphereConfig.HasMember("sun_size")) atmosphereConfig.m_sunSize = jsonAtmosphereConfig["sun_size"].GetFloat();

        if (jsonAtmosphereConfig.HasMember("density")) atmosphereConfig.m_density = jsonAtmosphereConfig["density"].GetFloat();
        if (jsonAtmosphereConfig.HasMember("multi_scatter_phase")) atmosphereConfig.m_multiScatterPhase = jsonAtmosphereConfig["multi_scatter_phase"].GetFloat();
        if (jsonAtmosphereConfig.HasMember("anisotropic_intenisty")) atmosphereConfig.m_anisotropicIntensity = jsonAtmosphereConfig["anisotropic_intenisty"].GetFloat();
        if (jsonAtmosphereConfig.HasMember("zenith_offset")) atmosphereConfig.m_zenithOffset = jsonAtmosphereConfig["zenith_offset"].GetFloat();

        if (jsonAtmosphereConfig.HasMember("night_density")) atmosphereConfig.m_nightDensity = jsonAtmosphereConfig["night_density"].GetFloat();
        if (jsonAtmosphereConfig.HasMember("night_zenith_y_clamp")) atmosphereConfig.m_nightZenithYClamp = jsonAtmosphereConfig["night_zenith_y_clamp"].GetFloat();

        m_sceneManager->getWritableSceneConfig().setAtmosphereConfig(atmosphereConfig);

        return LoadResult::eOk;
    }

    SceneLoader::LoadResult SceneLoader::loadObjectTracks(rapidjson::Value& sceneObject)
    {
        if (!sceneObject.HasMember("ObjectTracks"))
        {
            return LoadResult::eOk;
        }
        rapidjson::Value& objectTracks = sceneObject["ObjectTracks"];

        if (!objectTracks.IsArray())
        {
            utils::ErrorLogger::log("Parsing Scene JSON\n'ObjectTracks' is not a list");
            return LoadResult::eFailed;
        }

        std::string trackId;
        for (auto objectTrackItr = objectTracks.Begin(); objectTrackItr != objectTracks.End(); ++objectTrackItr)
        {
            entity::GameObjectTrack* track = new entity::GameObjectTrack();

            // Ensure we have a track id
            if (!objectTrackItr->HasMember("id"))
            {
                utils::ErrorLogger::log("Parsing Scene JSON\nObject track has no id");
                return LoadResult::eFailed;
            }
            trackId = (*objectTrackItr)["id"].GetString();
            
            // Ensure we have some valid nodes
            if (!objectTrackItr->HasMember("Nodes"))
            {
                utils::ErrorLogger::log("Parsing Scene JSON\nObject track with id '" + trackId + "' has no 'Nodes' tag");
                return LoadResult::eFailed;
            }
            rapidjson::Value& nodes = (*objectTrackItr)["Nodes"];
            if (!nodes.IsArray())
            {
                utils::ErrorLogger::log("Parsing Scene JSON\nObject track with id '" + trackId + "' tag 'Nodes' is not a list");
                return LoadResult::eFailed;
            }

            for (auto trackNodeItr = nodes.Begin(); trackNodeItr != nodes.End(); ++trackNodeItr)
            {
                if (!trackNodeItr->HasMember("position"))
                {
                    utils::ErrorLogger::log("Parsing Scene JSON\nObject track with id '" + trackId + "' has a node with no 'position' tag");
                    return LoadResult::eFailed;
                }
                XMFLOAT3 nodePos = getFloat3FromArray((*trackNodeItr)["position"].GetArray());

                if (!trackNodeItr->HasMember("look_position"))
                {
                    utils::ErrorLogger::log("Parsing Scene JSON\nObject track with id '" + trackId + "' has a node with no 'look_position' tag");
                    return LoadResult::eFailed;
                }
                XMFLOAT3 nodeLookAt = getFloat3FromArray((*trackNodeItr)["look_position"].GetArray());

                track->addTrackNode(entity::ObjectTrackNode(nodePos, nodeLookAt));
            }

            track->setId(trackId);
            track->generateMidPoints();
            
            m_sceneManager->addObjectTrack(track);
        }

        return LoadResult::eOk;
    }

    SceneLoader::LoadResult SceneLoader::loadGameObjects(rapidjson::Value& sceneObject)
    {
        if (!sceneObject.HasMember("GameObjects"))
        {
            utils::ErrorLogger::log("Parsing Scene JSON\nNo game object list found");
            return LoadResult::eFailed;
        }

        rapidjson::Value& gameObjects = sceneObject["GameObjects"];

        if (!gameObjects.IsArray())
        {
            utils::ErrorLogger::log("Parsing Scene JSON\n'GameObjects' was not a list");
            return LoadResult::eFailed;
        }

        entity::GameObject* gameObject = nullptr;

        // Base vars
        std::string id;
        std::string typeStr;
        entity::GameObject::Type type;
        bool floating;

        // Controller vars
        std::string controllerType;
        bool controllerActive;
        float controllerMoveSpeed;

        // Object track vars
        std::string objectTrackId;
        bool isFollowingTrack;
        float trackDelta;
        float trackSpeed;

        // Relative cameras vars
        XMFLOAT3 relativeCameraRelativePosition;

        // Renderable vars
        std::string modelPath;
        XMFLOAT3 scale;

        // Light vars
        XMFLOAT3 lightColour;

        // Point light vars
        float attenuationQuadratic;
        float attenuationLinear;
        float attenuationConstant;

        // Spot light vars
        float innerCutoff;
        float outerCutoff;

        // Physics vars
        float mass;
        float drag;

        // Camera vars
        float fov;
        float nearPlane;
        float farPlane;

        for (auto gameObjItr = gameObjects.Begin(); gameObjItr != gameObjects.End(); ++gameObjItr)
        {
            // Reset standard vars
            gameObject = nullptr;
            type = entity::GameObject::Type::eBase;
            floating = false;

            // Grab the id
            if (!gameObjItr->HasMember("id"))
            {
                utils::ErrorLogger::log("Parsing Scene JSON\nGame object had no id");
                continue;
            }
            id = (*gameObjItr)["id"].GetString();

            // Grab the type
            if (!gameObjItr->HasMember("type"))
            {
                utils::ErrorLogger::log("Parsing Scene JSON\nGame object with id: '" + id + "' had no type");
                continue;
            }
            typeStr = (*gameObjItr)["type"].GetString();
            type = entity::sc_gameObjectTypeStrings.at(typeStr);

            // Do type specific loading and initialisation
            switch (type)
            {
            case entity::GameObject::Type::eBase:
                gameObject = new entity::GameObject();
                break;
            case entity::GameObject::Type::eRenderable:
            {
                // Ensure 'RenderableData' exists
                if (!gameObjItr->HasMember("RenderableData"))
                {
                    utils::ErrorLogger::log("Parsing Scene JSON\nRenderable game object with id: '" + id + "' had no 'RenderableData' tag");
                    continue;
                }
                rapidjson::Value& renderableData = (*gameObjItr)["RenderableData"];

                // Ensure it stores a model
                if (!renderableData.HasMember("model_path"))
                {
                    utils::ErrorLogger::log("Parsing Scene JSON\nRenderable game object with id: '" + id + "' had no 'model_path' tag");
                    continue;
                }
                modelPath = renderableData["model_path"].GetString();

                // Load scale if its there
                scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
                if (renderableData.HasMember("scale"))
                {
                    scale = getFloat3FromArray(renderableData["scale"].GetArray());
                }

                entity::RenderableGameObject* renderable = new entity::RenderableGameObject();
                renderable->initialize(id, modelPath);
                renderable->setScale(scale);

                gameObject = renderable;
                break;
            }
            case entity::GameObject::Type::eCamera:
            {
                entity::CameraGameObject* camera = new entity::CameraGameObject();

                fov = 90.0f;
                nearPlane = 0.1f;
                farPlane = 2000.0f;

                // If 'CameraData' exists, load it's values
                if (gameObjItr->HasMember("CameraData"))
                {
                    rapidjson::Value& cameraData = (*gameObjItr)["CameraData"];

                    if (cameraData.HasMember("fov")) fov = cameraData["fov"].GetFloat();
                    if (cameraData.HasMember("near_plane")) nearPlane = cameraData["near_plane"].GetFloat();
                    if (cameraData.HasMember("far_plane")) farPlane = cameraData["far_plane"].GetFloat();

                    camera->setProjectionValues(fov, UserConfig::it().getWindowAspectRatio(), nearPlane, farPlane);
                }

                gameObject = camera;
                break;
            }
            case entity::GameObject::Type::eSpotLight:
            {
                entity::SpotLightGameObject* spotLight = new entity::SpotLightGameObject();

                if (gameObjItr->HasMember("SpotLightData"))
                {
                    rapidjson::Value& spotLightData = (*gameObjItr)["SpotLightData"];

                    if (spotLightData.HasMember("inner_cutoff"))
                    {
                        spotLight->setInnerCutoff(spotLightData["inner_cutoff"].GetFloat());
                    }

                    if (spotLightData.HasMember("outer_cutoff"))
                    {
                        spotLight->setOuterCutoff(spotLightData["outer_cutoff"].GetFloat());
                    }
                }

                gameObject = spotLight;
            }
            [[fallthrough]];
            case entity::GameObject::Type::ePointLight:
            {
                entity::PointLightGameObject* pointLight = nullptr;

                if (gameObject != nullptr) pointLight = dynamic_cast<entity::PointLightGameObject*>(gameObject);
                else pointLight = new entity::PointLightGameObject();

                if (gameObjItr->HasMember("PointLightData"))
                {
                    rapidjson::Value& pointLightData = (*gameObjItr)["PointLightData"];

                    if (pointLightData.HasMember("attenuation_constant"))
                    {
                        pointLight->setAttenuationConstant(pointLightData["attenuation_constant"].GetFloat());
                    }

                    if (pointLightData.HasMember("attenuation_linear"))
                    {
                        pointLight->setAttenuationLinear(pointLightData["attenuation_linear"].GetFloat());
                    }

                    if (pointLightData.HasMember("attenuation_quadratic"))
                    {
                        pointLight->setAttenuationQuadratic(pointLightData["attenuation_quadratic"].GetFloat());
                    }
                }

                gameObject = pointLight;
            }
            [[fallthrough]];
            case entity::GameObject::Type::eLight:
            {
                entity::LightGameObject* light = nullptr;

                if (gameObject != nullptr) light = dynamic_cast<entity::LightGameObject*>(gameObject);
                else light = new entity::LightGameObject();

                // If 'LightData' exists, load it's values
                if (gameObjItr->HasMember("LightData"))
                {
                    rapidjson::Value& lightData = (*gameObjItr)["LightData"];

                    if (lightData.HasMember("colour"))
                    {
                        light->setColour(getFloat3FromArray(lightData["colour"].GetArray()));
                    }
                }

                light->setLabel(id);
                light->initialize();

                gameObject = light;
                break;
            }
            case entity::GameObject::Type::ePhysics:
                continue;
                gameObject = new entity::PhysicsGameObject();
                break;
            }

            m_sceneManager->addGameObject(gameObject);

            // Grab the position
            if (gameObjItr->HasMember("position"))
            {
                XMFLOAT3 position = getFloat3FromArray((*gameObjItr)["position"].GetArray());
                gameObject->getWritableTransform().setPosition(position);
            }

            // Grab the orientation
            if (gameObjItr->HasMember("orientation"))
            {
                XMFLOAT4 orientation = getFloat4FromArray((*gameObjItr)["orientation"].GetArray());
                gameObject->getWritableTransform().setOrientationQuaternion(orientation);
            }

            // Load floating bool
            if (gameObjItr->HasMember("floating"))
            {
                gameObject->setFloating((*gameObjItr)["floating"].GetBool());
            }

            // Reset controller vars
            controllerActive = false;
            controllerType = "";
            controllerMoveSpeed = 1.0f;

            // Load controller if it has one
            if (gameObjItr->HasMember("Controller"))
            {
                rapidjson::Value& controller = (*gameObjItr)["Controller"];

                if (!controller.HasMember("type"))
                {
                    utils::ErrorLogger::log("Parsing Scene JSON\nGame object with id: '" + id + "' had a controller with no type");
                    continue;
                }
                controllerType = controller["type"].GetString();

                if (controller.HasMember("active"))
                {
                    controllerActive = controller["active"].GetBool();
                }

                if (controller.HasMember("move_speed"))
                {
                    controllerMoveSpeed = controller["move_speed"].GetFloat();
                }

                // TODO: SET OBJECT CONTROLLER
            }

            // Load any object tracks
            if (gameObjItr->HasMember("ObjectTracks"))
            {
                rapidjson::Value& objectTracks = (*gameObjItr)["ObjectTracks"];

                if (!objectTracks.IsArray())
                {
                    utils::ErrorLogger::log("Parsing Scene JSON\n'ObjectTracks' was not a list");
                    continue;
                }

                for (auto objectTrackItr = objectTracks.Begin(); objectTrackItr != objectTracks.End(); ++objectTrackItr)
                {
                    // Reset object track vars
                    objectTrackId = "";
                    isFollowingTrack = false;
                    trackDelta = 0.0f;
                    trackSpeed = 1.0f;

                    if (!objectTrackItr->HasMember("id"))
                    {
                        utils::ErrorLogger::log("Parsing Scene JSON\nGame object with id: '" + id + "' had an object track with no id");
                        continue;
                    }
                    objectTrackId = (*objectTrackItr)["id"].GetString();

                    if (objectTrackItr->HasMember("is_following"))
                    {
                        isFollowingTrack = (*objectTrackItr)["is_following"].GetBool();
                    }

                    if (objectTrackItr->HasMember("is_following"))
                    {
                        trackDelta = (*objectTrackItr)["track_delta"].GetFloat();
                    }

                    if (objectTrackItr->HasMember("is_following"))
                    {
                        trackSpeed = (*objectTrackItr)["track_speed"].GetFloat();
                    }

                    auto trackItr = m_sceneManager->getObjectTracks().find(objectTrackId);
                    if (trackItr != m_sceneManager->getObjectTracks().end())
                    {
                        gameObject->setObjectTrack(trackItr->second);
                        gameObject->setFollowingObjectTrack(isFollowingTrack);
                        gameObject->setObjectTrackDelta(trackDelta);
                        gameObject->setObjectTrackSpeed(trackSpeed);
                    }
                    else
                    {
                        utils::ErrorLogger::log("Parsing Scene JSON\nGame object with id: '" + id + "' had an object track with no id");
                    }
                }
            }

            // Load any relative cameras
            if (gameObjItr->HasMember("RelativeCameras"))
            {
                rapidjson::Value& relativeCameras = (*gameObjItr)["RelativeCameras"];

                if (!relativeCameras.IsArray())
                {
                    utils::ErrorLogger::log("Parsing Scene JSON\n'RelativeCameras' was not a list");
                    continue;
                }

                for (auto relativeCameraItr = relativeCameras.Begin(); relativeCameraItr != relativeCameras.End(); ++relativeCameraItr)
                {
                    if (!relativeCameraItr->HasMember("relative_position"))
                    {
                        utils::ErrorLogger::log("Parsing Scene JSON\nGame object with id: '" + id + "' had a relative camera with no relative position");
                        continue;
                    }
                    relativeCameraRelativePosition = getFloat3FromArray((*relativeCameraItr)["relative_position"].GetArray());

                    // TODO: ADD RELATIVE CAMERA TO LIST
                }
            }
        }

        return LoadResult::eOk;
    }

    DirectX::XMFLOAT3 SceneLoader::getFloat3FromArray(rapidjson::Value::Array arrayObject)
    {
        XMFLOAT3 returnValue = XMFLOAT3(0.0f, 0.0f, 0.0f);
        int index = 0;
        for (auto itr = arrayObject.Begin(); itr != arrayObject.End(); ++itr)
        {
            if (index == 0) returnValue.x = itr->GetFloat();
            else if (index == 1) returnValue.y = itr->GetFloat();
            else if (index == 2) returnValue.z = itr->GetFloat();
            else return returnValue;

            ++index;
        }

        return returnValue;
    }

    DirectX::XMFLOAT4 SceneLoader::getFloat4FromArray(rapidjson::Value::Array arrayObject)
    {
        XMFLOAT4 returnValue = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
        int index = 0;
        for (auto itr = arrayObject.Begin(); itr != arrayObject.End(); ++itr)
        {
            if (index == 0) returnValue.x = itr->GetFloat();
            else if (index == 1) returnValue.y = itr->GetFloat();
            else if (index == 2) returnValue.z = itr->GetFloat();
            else if (index == 3) returnValue.w = itr->GetFloat();
            else return returnValue;

            ++index;
        }

        return returnValue;
    }
}