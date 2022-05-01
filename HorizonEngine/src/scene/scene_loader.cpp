#include "scene_loader.h"

#include <fstream>
#include <sstream>
#include <algorithm>

#include "../utils/error_logger.h"
#include "../utils/json_helpers.h"

#include "../user_config.h"

#include "scene_manager.h"

namespace hrzn::scene
{
    SceneLoader::SceneLoader()
    {
    }

    SceneLoader::~SceneLoader()
    {
    }

    void SceneLoader::combineResults(const LoadResult& compare, LoadResult& storage)
    {
        if (compare > storage) storage = compare;
    }

    SceneLoader::LoadResult SceneLoader::loadScene(const char* sceneName)
    {
        std::string sceneFilePath = std::string("res/scenes/") + sceneName + ".hrzn";

        LoadResult loadResult = LoadResult::eOk;
        rapidjson::Document document;

        if (utils::JSONHelpers::parse(sceneFilePath, document) == LoadResult::eFailed)
        {
            return LoadResult::eFailed;
        }

        // Ensure it has a scene object
        if (!document.HasMember("Scene"))
        {
            utils::ErrorLogger::log("Parsing Scene JSON\nNo scene tag found");
            return LoadResult::eFailed;
        }
        rapidjson::Value& sceneObject = document["Scene"];

        combineResults(loadMeta(sceneObject), loadResult);

        combineResults(loadConfigs(sceneObject), loadResult);

        combineResults(loadObjectTracks(sceneObject), loadResult);

        combineResults(loadGameObjects(sceneObject), loadResult);

        return loadResult;
    }

    void SceneLoader::saveScene(const char* sceneName)
    {
        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

        writer.SetMaxDecimalPlaces(7);

        writer.StartObject();
        writer.String("Scene");
        writer.StartObject();

        saveMeta(writer);
        saveConfigs(writer);

        saveObjectTracks(writer);

        saveGameObjects(writer);

        writer.EndObject();
        writer.EndObject();

        std::string sceneFileName = (std::string)"res/scenes/" + sceneName + ".hrzn";
        std::ofstream outStream(sceneFileName.c_str());
        if (outStream)
        {
            outStream << buffer.GetString();

            outStream.close();
        }
    }

    SceneLoader::LoadResult SceneLoader::loadMeta(rapidjson::Value& sceneObject)
    {
        if (!sceneObject.HasMember("SceneMeta"))
        {
            utils::ErrorLogger::log("Parsing Scene JSON\nNo 'SceneMeta' tag found");
            return LoadResult::eFailed;
        }
        rapidjson::Value& jsonSceneMeta = sceneObject["SceneMeta"];

        if (jsonSceneMeta.HasMember("scene_name")) SceneManager::it().getWritableSceneMeta().m_sceneName = jsonSceneMeta["scene_name"].GetString();
        if (jsonSceneMeta.HasMember("author_name")) SceneManager::it().getWritableSceneMeta().m_authorName = jsonSceneMeta["author_name"].GetString();
        if (jsonSceneMeta.HasMember("author_email")) SceneManager::it().getWritableSceneMeta().m_authorEmail = jsonSceneMeta["author_email"].GetString();
        if (jsonSceneMeta.HasMember("date_created")) SceneManager::it().getWritableSceneMeta().m_dateCreated = jsonSceneMeta["date_created"].GetString();
        if (jsonSceneMeta.HasMember("date_modified")) SceneManager::it().getWritableSceneMeta().m_dateModified = jsonSceneMeta["date_modified"].GetString();

        return LoadResult::eOk;
    }

    void SceneLoader::saveMeta(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
    {
        writer.String("SceneMeta");

        writer.StartObject();

        const SceneMeta& sceneMeta = SceneManager::it().getSceneMeta();

        writer.String("scene_name"); writer.String(sceneMeta.m_sceneName.c_str());
        writer.String("author_name"); writer.String(sceneMeta.m_authorName.c_str());
        writer.String("author_email"); writer.String(sceneMeta.m_authorEmail.c_str());
        writer.String("date_created"); writer.String(sceneMeta.m_dateCreated.c_str());
        writer.String("date_modified"); writer.String(sceneMeta.m_dateModified.c_str());
        

        writer.EndObject();
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
        combineResults(loadTerrainConfig(sceneConfig), result);

        return result;
    }

    void SceneLoader::saveConfigs(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
    {
        writer.String("SceneConfig");

        writer.StartObject();

        saveTimeConfig(writer);
        saveOceanConfig(writer);
        saveCloudConfig(writer);
        saveAtmosphereConfig(writer);
        saveTerrainConfig(writer);

        writer.EndObject();
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

        SceneManager::it().getWritableSceneConfig().setTimeConfig(timeConfig);

        return LoadResult::eOk;
    }

    void SceneLoader::saveTimeConfig(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
    {
        writer.String("TimeConfig");

        writer.StartObject();

        config::TimeConfig& timeConfig = SceneManager::it().getWritableSceneConfig().getWritableTimeConfig();

        writer.String("start_time"); writer.Double(timeConfig.m_startTime);
        writer.String("day_night_cycle"); writer.Bool(timeConfig.m_dayNightCycle);
        writer.String("time_paused"); writer.Bool(timeConfig.m_timePaused);
        writer.String("day_duration"); writer.Double(timeConfig.m_dayDuration);

        writer.EndObject();
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

        SceneManager::it().getWritableSceneConfig().setOceanConfig(oceanConfig);

        return LoadResult::eOk;
    }

    void SceneLoader::saveOceanConfig(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
    {
        writer.String("OceanConfig");

        writer.StartObject();

        config::OceanConfig& oceanConfig = SceneManager::it().getWritableSceneConfig().getWritableOceanConfig();

        writer.String("enabled"); writer.Bool(oceanConfig.m_enabled);

        writer.String("wave_count"); writer.Int(oceanConfig.m_waveCount);

        writer.String("wave_scale"); writer.Double(oceanConfig.m_waveScale);
        writer.String("wave_scale_multiplier"); writer.Double(oceanConfig.m_waveScaleMultiplier);

        writer.String("wave_period"); writer.Double(oceanConfig.m_wavePeriod);
        writer.String("wave_speed"); writer.Double(oceanConfig.m_waveSpeed);
        writer.String("wave_seed"); writer.Double(oceanConfig.m_waveSeed);

        writer.String("foam_start"); writer.Double(oceanConfig.m_foamStart);
        writer.String("colour_change_start"); writer.Double(oceanConfig.m_colourChangeStart);

        writer.EndObject();
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

        SceneManager::it().getWritableSceneConfig().setCloudConfig(cloudConfig);

        return LoadResult::eOk;
    }

    void SceneLoader::saveCloudConfig(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
    {
        writer.String("CloudConfig");

        writer.StartObject();

        config::CloudConfig& cloudConfig = SceneManager::it().getWritableSceneConfig().getWritableCloudConfig();

        writer.String("enabled"); writer.Bool(cloudConfig.m_enabled);

        writer.String("light_absorbtion_through_clouds"); writer.Double(cloudConfig.m_lightAbsorbtionThroughClouds);
        writer.String("light_absorbtion_towards_sun"); writer.Double(cloudConfig.m_lightAbsorbtionTowardsSun);
        writer.String("phase_factor"); writer.Double(cloudConfig.m_phaseFactor);
        writer.String("darkness_threshold"); writer.Double(cloudConfig.m_darknessThreshold);

        writer.String("cloud_speed"); writer.Double(cloudConfig.m_cloudSpeed);
        writer.String("cloud_coverage"); writer.Double(cloudConfig.m_cloudCoverage);
        writer.String("cloud_height"); writer.Double(cloudConfig.m_cloudHeight);

        writer.String("num_steps"); writer.Int(cloudConfig.m_numSteps);
        writer.String("step_size"); writer.Double(cloudConfig.m_stepSize);

        writer.EndObject();
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

        SceneManager::it().getWritableSceneConfig().setAtmosphereConfig(atmosphereConfig);

        return LoadResult::eOk;
    }

    void SceneLoader::saveAtmosphereConfig(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
    {
        writer.String("AtmosphereConfig");

        writer.StartObject();

        config::AtmosphereConfig& atmosphereConfig = SceneManager::it().getWritableSceneConfig().getWritableAtmosphereConfig();

        writer.String("sun_size"); writer.Double(atmosphereConfig.m_sunSize);

        writer.String("density"); writer.Double(atmosphereConfig.m_density);
        writer.String("multi_scatter_phase"); writer.Double(atmosphereConfig.m_multiScatterPhase);
        writer.String("anisotropic_intenisty"); writer.Double(atmosphereConfig.m_anisotropicIntensity);
        writer.String("zenith_offset"); writer.Double(atmosphereConfig.m_zenithOffset);

        writer.String("night_density"); writer.Double(atmosphereConfig.m_nightDensity);
        writer.String("night_zenith_y_clamp"); writer.Double(atmosphereConfig.m_nightZenithYClamp);

        writer.EndObject();
    }

    /***********************************************
    
    MARKING SCHEME: Terrain construction
    COMMENT INDEX: 1
    DESCRIPTION: This is where the terrain setup info is loaded from the scene
                 json file (generation type, where it is in the level etc)

    ***********************************************/

    SceneLoader::LoadResult SceneLoader::loadTerrainConfig(rapidjson::Value& sceneConfig)
    {
        if (!sceneConfig.HasMember("TerrainConfig"))
        {
            utils::ErrorLogger::log("Parsing Scene JSON\nNo 'TerrainConfig' tag found");
            return LoadResult::eFailed;
        }
        rapidjson::Value& jsonTerrainConfig = sceneConfig["TerrainConfig"];

        config::TerrainConfig terrainConfig;

        if (jsonTerrainConfig.HasMember("enabled")) terrainConfig.m_enabled = jsonTerrainConfig["enabled"].GetBool();

        if (jsonTerrainConfig.HasMember("dimension_type"))
        {
            std::string dimensionTypeString = jsonTerrainConfig["dimension_type"].GetString();

            if (dimensionTypeString == "2d")
            {
                terrainConfig.m_dimensionType = config::TerrainConfig::DimensionType::e2D;
            }
            else if (dimensionTypeString == "3d")
            {
                terrainConfig.m_dimensionType = config::TerrainConfig::DimensionType::e3D;
            }
        }

        if (jsonTerrainConfig.HasMember("is_infinite")) terrainConfig.m_isInfinite = jsonTerrainConfig["is_infinite"].GetBool();
        if (jsonTerrainConfig.HasMember("chunk_scale")) terrainConfig.m_chunkScale = jsonTerrainConfig["chunk_scale"].GetFloat();
        if (jsonTerrainConfig.HasMember("origin_position"))
        {
            XMFLOAT3 origin = utils::JSONHelpers::getFloat3FromArray(jsonTerrainConfig["origin_position"].GetArray());
            terrainConfig.m_originPosition = maths::Vec3f(origin.x, origin.y, origin.z);
        }

        // 2D Config vars
        if (terrainConfig.m_dimensionType == config::TerrainConfig::DimensionType::e2D)
        {
            // Generation type
            if (jsonTerrainConfig.HasMember("2d_generation_type"))
            {
                std::string generationTypeString = jsonTerrainConfig["2d_generation_type"].GetString();

                if (generationTypeString == "heightmap")
                {
                    terrainConfig.m_generationType2D = config::TerrainConfig::GenerationType2D::eHeightmap;

                    if (jsonTerrainConfig.HasMember("heightmap_path")) terrainConfig.m_heightmapPath = jsonTerrainConfig["heightmap_path"].GetString();
                }
                else if (generationTypeString == "diamond_square")
                {
                    terrainConfig.m_generationType2D = config::TerrainConfig::GenerationType2D::eDiamondSquare;
                }
                else if (generationTypeString == "fault_line")
                {
                    terrainConfig.m_generationType2D = config::TerrainConfig::GenerationType2D::eFaultLine;
                }
                else if (generationTypeString == "circle")
                {
                    terrainConfig.m_generationType2D = config::TerrainConfig::GenerationType2D::eCircle;
                }
            }
        }
        // 3D Config vars
        else if (terrainConfig.m_dimensionType == config::TerrainConfig::DimensionType::e3D)
        {
            // Generation type
            if (jsonTerrainConfig.HasMember("3d_generation_type"))
            {
                std::string generationTypeString = jsonTerrainConfig["3d_generation_type"].GetString();

                if (generationTypeString == "default")
                {
                    terrainConfig.m_generationType3D = config::TerrainConfig::GenerationType3D::eDefault;
                }
            }
        }

        SceneManager::it().getWritableSceneConfig().setTerrainConfig(terrainConfig);

        return LoadResult::eOk;
    }

    void SceneLoader::saveTerrainConfig(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
    {
        writer.String("TerrainConfig");

        writer.StartObject();

        config::TerrainConfig& terrainConfig = SceneManager::it().getWritableSceneConfig().getWritableTerrainConfig();

        writer.String("enabled"); writer.Bool(terrainConfig.m_enabled);

        writer.String("dimension_type"); 

        if (terrainConfig.m_dimensionType == config::TerrainConfig::DimensionType::e2D) writer.String("2d");
        else if (terrainConfig.m_dimensionType == config::TerrainConfig::DimensionType::e3D) writer.String("3d");
        else writer.String("undefined");

        writer.String("is_infinite"); writer.Bool(terrainConfig.m_isInfinite);
        writer.String("chunk_scale"); writer.Double(terrainConfig.m_chunkScale);
        writer.String("origin_position"); utils::JSONHelpers::writeFloat3(terrainConfig.m_originPosition.getAsXMFLOAT3(), writer);

        writer.String("2d_generation_type");
        if (terrainConfig.m_generationType2D == config::TerrainConfig::GenerationType2D::eHeightmap) writer.String("heightmap");
        else if (terrainConfig.m_generationType2D == config::TerrainConfig::GenerationType2D::eDiamondSquare) writer.String("diamond_square");
        else if (terrainConfig.m_generationType2D == config::TerrainConfig::GenerationType2D::eFaultLine) writer.String("fault_line");
        else if (terrainConfig.m_generationType2D == config::TerrainConfig::GenerationType2D::eCircle) writer.String("circle");

        writer.String("heightmap_path"); writer.String(terrainConfig.m_heightmapPath.c_str());

        writer.String("3d_generation_type");
        if (terrainConfig.m_generationType3D == config::TerrainConfig::GenerationType3D::eDefault) writer.String("default");

        writer.EndObject();
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
                XMFLOAT3 nodePos = utils::JSONHelpers::getFloat3FromArray((*trackNodeItr)["position"].GetArray());

                if (!trackNodeItr->HasMember("look_position"))
                {
                    utils::ErrorLogger::log("Parsing Scene JSON\nObject track with id '" + trackId + "' has a node with no 'look_position' tag");
                    return LoadResult::eFailed;
                }
                XMFLOAT3 nodeLookAt = utils::JSONHelpers::getFloat3FromArray((*trackNodeItr)["look_position"].GetArray());

                track->addTrackNode(entity::ObjectTrackNode(nodePos, nodeLookAt));
            }

            track->setId(trackId);
            track->generateMidPoints();
            
            SceneManager::it().addObjectTrack(track);
        }

        return LoadResult::eOk;
    }

    void SceneLoader::saveObjectTracks(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
    {
        writer.String("ObjectTracks");

        writer.StartArray();

        auto& objectTracks = SceneManager::it().getObjectTracks();
        for (const auto& objectTrack : objectTracks)
        {
            writer.StartObject();

            writer.String("id"); writer.String(objectTrack.second->getId().c_str());
            writer.String("Nodes");

            writer.StartArray();

            const auto& objectTrackNodes = objectTrack.second->getTrackNodes();
            for (const auto& objectTrackNode : objectTrackNodes)
            {
                writer.StartObject();

                writer.String("position"); utils::JSONHelpers::writeFloat3(objectTrackNode.m_position, writer);
                writer.String("look_position"); utils::JSONHelpers::writeFloat3(objectTrackNode.m_lookPoint, writer);

                writer.EndObject();
            }

            writer.EndArray();

            writer.EndObject();
        }

        writer.EndArray();
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
        std::string controllerTypeStr;
        entity::ControllerType controllerType;
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
            type = entity::sc_gameObjectStringToType.at(typeStr);

            // Do type specific loading and initialisation
            switch (type)
            {
            case entity::GameObject::Type::eBase:
                gameObject = new entity::GameObject();
                break;
            case entity::GameObject::Type::eSkinned:
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
                    scale = utils::JSONHelpers::getFloat3FromArray(renderableData["scale"].GetArray());
                }

                entity::RenderableGameObject* renderable = nullptr;
                if (type == entity::GameObject::Type::eSkinned)
                {
                    renderable = new entity::SkinnedGameObject();
                    renderable->initialise(id, modelPath);
                }
                else
                {
                    renderable = new entity::RenderableGameObject();
                    renderable->initialise(id, modelPath);
                }
                
                renderable->setScale(scale);
                gameObject = renderable;
                
                break;
            }
            case entity::GameObject::Type::eCamera:
            {
                entity::CameraGameObject* camera = new entity::CameraGameObject();

                camera->setLabel(id);

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

                    if (cameraData.HasMember("is_active_cam"))
                    {
                        if (cameraData["is_active_cam"].GetBool())
                        {
                            SceneManager::it().setActiveCamera(camera);
                        }
                    }
                    
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
                        light->setColour(utils::JSONHelpers::getFloat3FromArray(lightData["colour"].GetArray()));
                    }
                }

                light->setLabel(id);
                light->initialise();

                gameObject = light;
                break;
            }
            case entity::GameObject::Type::ePhysics:
                continue;
                gameObject = new entity::PhysicsGameObject();
                break;
            }

            SceneManager::it().addGameObject(gameObject);

            // Grab the position
            if (gameObjItr->HasMember("position"))
            {
                XMFLOAT3 position = utils::JSONHelpers::getFloat3FromArray((*gameObjItr)["position"].GetArray());
                gameObject->getWritableTransform().setPosition(position);
            }

            // Grab the orientation
            if (gameObjItr->HasMember("orientation"))
            {
                XMFLOAT4 orientation = utils::JSONHelpers::getFloat4FromArray((*gameObjItr)["orientation"].GetArray());
                gameObject->getWritableTransform().setOrientationQuaternion(orientation);
            }

            // Load floating bool
            if (gameObjItr->HasMember("floating"))
            {
                gameObject->setFloating((*gameObjItr)["floating"].GetBool());
            }

            // Reset controller vars
            controllerActive = false;
            controllerTypeStr = "";
            controllerType = entity::ControllerType::eInvalid;
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
                controllerTypeStr = controller["type"].GetString();

                auto controllerTypeItr = entity::sc_controllerStringToType.find(controllerTypeStr);
                if (controllerTypeItr == entity::sc_controllerStringToType.end())
                {
                    utils::ErrorLogger::log("Parsing Scene JSON\nGame object with id: '" + id + "' had a controller with an invalid type");
                    continue;
                }
                controllerType = controllerTypeItr->second;

                if (controller.HasMember("active"))
                {
                    controllerActive = controller["active"].GetBool();
                }

                if (controller.HasMember("move_speed"))
                {
                    controllerMoveSpeed = controller["move_speed"].GetFloat();
                }

                SceneManager::it().getWritableControllerManager()->addController(gameObject, controllerType, controllerMoveSpeed, controllerActive);
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

                    auto trackItr = SceneManager::it().getObjectTracks().find(objectTrackId);
                    if (trackItr != SceneManager::it().getObjectTracks().end())
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

                    relativeCameraRelativePosition = utils::JSONHelpers::getFloat3FromArray((*relativeCameraItr)["relative_position"].GetArray());

                    gameObject->getWritableRelativePositions().push_back(relativeCameraRelativePosition);
                }
            }
        }

        return LoadResult::eOk;
    }

    void SceneLoader::saveGameObjects(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
    {
        writer.String("GameObjects");

        writer.StartArray();

        const auto& gameObjects = scene::SceneManager::it().getObjectMap();
        for (const auto& gameObject : gameObjects)
        {
            // Nullptr object
            if (gameObject.second == nullptr)
            {
                continue;
            }

            // Engine object
            if (gameObject.second->getLabel()[0] == '$')
            {
                continue;
            }

            writer.StartObject();

            entity::GameObject* gameObjectPtr = gameObject.second;

            writer.String("id"); writer.String(gameObjectPtr->getLabel().c_str());
            writer.String("type"); writer.String(entity::sc_gameObjectTypeToString.at(gameObjectPtr->getType()).c_str());
            writer.String("position"); utils::JSONHelpers::writeFloat3(gameObjectPtr->getTransform().getPositionFloat3(), writer);
            writer.String("orientation"); utils::JSONHelpers::writeFloat4(gameObjectPtr->getTransform().getOrientationFloat4(), writer);

            if (gameObjectPtr->getFloating())
            {
                writer.String("floating"); writer.Bool(true);
            }

            if (auto* light = dynamic_cast<entity::LightGameObject*>(gameObjectPtr))
            {
                writer.String("LightData");
                writer.StartObject();

                writer.String("colour"); utils::JSONHelpers::writeFloat3(light->getColour(), writer);

                writer.EndObject();

                if (auto* pointLight = dynamic_cast<entity::PointLightGameObject*>(light))
                {
                    writer.String("PointLightData");
                    writer.StartObject();

                    writer.String("attenuation_constant"); writer.Double(*pointLight->getAttenuationConstantPtr());
                    writer.String("attenuation_linear"); writer.Double(*pointLight->getAttenuationLinearPtr());
                    writer.String("attenuation_quadratic"); writer.Double(*pointLight->getAttenuationQuadraticPtr());

                    writer.EndObject();

                    if (auto* spotLight = dynamic_cast<entity::SpotLightGameObject*>(pointLight))
                    {
                        writer.String("SpotLightData");
                        writer.StartObject();

                        writer.String("inner_cutoff"); writer.Double(spotLight->getInnerCutoff());
                        writer.String("outer_cutoff"); writer.Double(spotLight->getOuterCutoff());

                        writer.EndObject();
                    }
                }
            }
            else if (auto* renderable = dynamic_cast<entity::RenderableGameObject*>(gameObjectPtr))
            {
                writer.String("RenderableData");
                writer.StartObject();

                writer.String("model_path"); writer.String(renderable->getModel().getPath().c_str());
                writer.String("scale"); utils::JSONHelpers::writeFloat3(renderable->getScale(), writer);
                    
                writer.EndObject();
            }

            if (auto* camera = dynamic_cast<entity::CameraGameObject*>(gameObjectPtr))
            {
                writer.String("CameraData");
                writer.StartObject();

                writer.String("fov"); writer.Double(camera->getFOV());
                writer.String("near_plane"); writer.Double(camera->getNearPlane());
                writer.String("far_plane"); writer.Double(camera->getFarPlane());
                writer.String("is_active_cam"); writer.Bool(&(scene::SceneManager::it().getActiveCamera()) == camera);

                writer.EndObject();
            }

            if (gameObjectPtr->getController() != nullptr)
            {
                writer.String("Controller");
                writer.StartObject();

                writer.String("active"); writer.Bool(gameObjectPtr->getController()->isActive());
                writer.String("type"); writer.String(entity::sc_controllerTypeToString.at(gameObjectPtr->getController()->getType()).c_str());
                writer.String("move_speed"); writer.Double(gameObjectPtr->getController()->getMoveSpeed());

                writer.EndObject();
            }

            auto& relativePositions = gameObjectPtr->getRelativePositions();
            if (!relativePositions.empty())
            {
                writer.String("RelativeCameras");
                writer.StartArray();

                for (int relativePositionIndex = 0; relativePositionIndex < relativePositions.size(); ++relativePositionIndex)
                {
                    writer.StartObject();

                    writer.String("relative_position"); utils::JSONHelpers::writeFloat3(relativePositions[relativePositionIndex], writer);

                    writer.EndObject();
                }

                writer.EndArray();
            }

            if (gameObjectPtr->getObjectTrack() != nullptr)
            {
                writer.String("ObjectTracks");
                writer.StartArray();
                writer.StartObject();

                writer.String("id"); writer.String(gameObjectPtr->getObjectTrack()->getId().c_str());
                writer.String("is_following"); writer.Bool(gameObjectPtr->isFollowingObjectTrack());
                writer.String("track_delta"); writer.Double(gameObjectPtr->getObjectTrackDelta());
                writer.String("track_speed"); writer.Double(gameObjectPtr->getObjectTrackSpeed());

                writer.EndObject();
                writer.EndArray();
            }

            writer.EndObject();
        }

        writer.EndArray();
    }
}