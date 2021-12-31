#pragma once

#include <string.h>
#include <iostream>

#include "rapidjson/reader.h"
#include "rapidjson/document.h"

#include <DirectXMath.h>

// fwd declarations
namespace hrzn::scene
{
    class SceneManager;
}

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
        SceneLoader(SceneManager* sceneManager);
        ~SceneLoader();

        static void combineResults(const LoadResult& compare, LoadResult& storage);

        LoadResult loadScene(std::string sceneName);

        LoadResult loadMeta(rapidjson::Value& sceneObject);

        LoadResult loadConfigs(rapidjson::Value& sceneObject);

        LoadResult loadTimeConfig(rapidjson::Value& sceneConfig);
        LoadResult loadOceanConfig(rapidjson::Value& sceneConfig);
        LoadResult loadCloudConfig(rapidjson::Value& sceneConfig);
        LoadResult loadAtmosphereConfig(rapidjson::Value& sceneConfig);

        LoadResult loadObjectTracks(rapidjson::Value& sceneObject);
        LoadResult loadGameObjects(rapidjson::Value& sceneObject);

    private:
        SceneManager*       m_sceneManager;
    };
}

//struct JSONHandler
//{
//    bool Null()
//    {
//        std::cout << "Null()" << "\n";
//        return true;
//    }
//
//    bool Bool(bool b)
//    {
//        std::cout << "Bool(" << std::boolalpha << b << ")" << "\n";
//        return true;
//    }
//
//    bool Int(int i)
//    {
//        std::cout << "Int(" << i << ")" << "\n";
//        return true;
//    }
//
//    bool Uint(unsigned u)
//    {
//        std::cout << "Uint(" << u << ")" << "\n";
//        return true;
//    }
//
//    bool Int64(int64_t i)
//    {
//        std::cout << "Int64(" << i << ")" << "\n";
//        return true;
//    }
//
//    bool Uint64(uint64_t u)
//    {
//        std::cout << "Uint64(" << u << ")" << "\n";
//        return true;
//    }
//
//    bool Double(double d)
//    {
//        std::cout << "Double(" << d << ")" << "\n";
//        return true;
//    }
//
//    bool RawNumber(const char* str, rapidjson::SizeType length, bool copy)
//    {
//        std::cout << "Number(" << str << ", " << length << ", " << std::boolalpha << copy << ")" << "\n";
//        return true;
//    }
//
//    bool String(const char* str, rapidjson::SizeType length, bool copy)
//    {
//        std::cout << "String(" << str << ", " << length << ", " << std::boolalpha << copy << ")" << "\n";
//        return true;
//    }
//
//    bool StartObject()
//    {
//        std::cout << "StartObject()" << "\n";
//        return true;
//    }
//
//    bool Key(const char* str, rapidjson::SizeType length, bool copy)
//    {
//        std::cout << "Key(" << str << ", " << length << ", " << std::boolalpha << copy << ")" << "\n";
//        return true;
//    }
//
//    bool EndObject(rapidjson::SizeType memberCount)
//    {
//        std::cout << "EndObject(" << memberCount << ")" << "\n";
//        return true;
//    }
//
//    bool StartArray()
//    {
//        std::cout << "StartArray()" << "\n";
//        return true;
//    }
//
//    bool EndArray(rapidjson::SizeType elementCount)
//    {
//        std::cout << "EndArray(" << elementCount << ")" << "\n";
//        return true;
//    }
//};
