#pragma once

#include <string.h>
#include <iostream>

#include <rapidjson/document.h>
#include <rapidjson/reader.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

#include <DirectXMath.h>

namespace hrzn::scene
{
    class SceneLoader
    {
    public:
        enum class LoadResult
        {
            eOk,
            eIncomplete,
            eFailed
        };

    public:
        SceneLoader();
        ~SceneLoader();

        static void combineResults(const LoadResult& compare, LoadResult& storage);

        LoadResult loadScene(const char* sceneName);
        void       saveScene(const char* sceneName);

        LoadResult loadMeta(rapidjson::Value& sceneObject);
        void       saveMeta(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

        LoadResult loadConfigs(rapidjson::Value& sceneObject);
        void       saveConfigs(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

        LoadResult loadTimeConfig(rapidjson::Value& sceneConfig);
        void       saveTimeConfig(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
        LoadResult loadOceanConfig(rapidjson::Value& sceneConfig);
        void       saveOceanConfig(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
        LoadResult loadCloudConfig(rapidjson::Value& sceneConfig);
        void       saveCloudConfig(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
        LoadResult loadAtmosphereConfig(rapidjson::Value& sceneConfig);
        void       saveAtmosphereConfig(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

        LoadResult loadObjectTracks(rapidjson::Value& sceneObject);
        void       saveObjectTracks(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

        LoadResult loadGameObjects(rapidjson::Value& sceneObject);
        void       saveGameObjects(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
    };
}
