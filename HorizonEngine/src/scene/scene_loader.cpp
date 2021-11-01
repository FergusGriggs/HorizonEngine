#include "scene_loader.h"

#include <fstream>
#include <sstream>
#include <algorithm>

#include "../utils/error_logger.h"

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

    bool SceneLoader::loadScene(std::string sceneName)
    {
        std::string sceneFilePath = "res/scenes/";
        sceneFilePath += sceneName + ".hrzn";

        std::ifstream sceneFile(sceneFilePath.c_str());
        std::stringstream buffer;
        buffer << sceneFile.rdbuf();
        std::string sceneFileStr = buffer.str();

        //const char json[] = " { \"hello\" : \"world\", \"t\" : true , \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4] } ";

        m_document.Parse(buffer.str().c_str());
        if (m_document.HasParseError())
        {
            rapidjson::ParseErrorCode error = m_document.GetParseError();
            size_t errorOffset = m_document.GetErrorOffset();

            int errorStart = std::max(0, static_cast<int>(errorOffset) - 20);
            int errorEnd = std::min(strlen(buffer.str().c_str()), errorOffset + 20);

            int beforeLen = errorOffset - errorStart;
            int afterLen = errorEnd - errorOffset;

            std::string errorString = sceneFileStr.substr(errorStart, beforeLen) + "[HERE]" + sceneFileStr.substr(errorOffset, afterLen);
            errorString.erase(std::remove(errorString.begin(), errorString.end(), '\n'), errorString.end());
            errorString.erase(std::remove(errorString.begin(), errorString.end(), '\t'), errorString.end());

            int lineNumber = utils::string_helpers::getLineNumberFromOffset(sceneFileStr, errorOffset);

            std::string finalErrorString = "Parsing Scene JSON\nCode: '" + std::to_string(error) + "'\nLine Num: '" + std::to_string(lineNumber) + "'\nError String: '" + errorString + "'\n";

            utils::ErrorLogger::log(finalErrorString);
        }

        if (!m_document.HasMember("Scene"))
        {
            utils::ErrorLogger::log("Parsing Scene JSON\nNo scene tag found");
            return false;
        }

        auto& sceneObject = m_document["Scene"];

        if (!m_document.HasMember("GameObjects"))
        {
            utils::ErrorLogger::log("Parsing Scene JSON\nNo game object list found");
            return false;
        }

        auto& gameObjects = sceneObject["GameObjects"];

        if (!gameObjects.IsArray())
        {
            utils::ErrorLogger::log("Parsing Scene JSON\n'GameObjects' tag was not a list");
            return false;
        }

        for (auto gameObjItr = gameObjects.Begin(); gameObjItr != gameObjects.End(); ++gameObjItr)
        {
            // Grab the id
            std::string id;
            if (!gameObjItr->HasMember("id"))
            {
                utils::ErrorLogger::log("Parsing Scene JSON\nGame object had no id");
                return false;
            }
            id = (*gameObjItr)["id"].GetString();

            // Grab the type
            std::string type;
            if (!gameObjItr->HasMember("type"))
            {
                utils::ErrorLogger::log("Parsing Scene JSON\nGame object with id: '" + id + "' had no type");
                return false;
            }
            type = (*gameObjItr)["type"].GetString();

            // Grab the position
            XMFLOAT3 position;
            if (!gameObjItr->HasMember("position"))
            {
                utils::ErrorLogger::log("Parsing Scene JSON\nGame object with id: '" + id + "' had no position");
                return false;
            }
            position = getFloat3FromArray((*gameObjItr)["position"].GetArray());


            entity::RenderableGameObject* newObject = new entity::RenderableGameObject();
            m_sceneManager->addGameObject(newObject);
        }

        /*for (auto itr = m_document.MemberBegin(); itr != m_document.MemberEnd(); ++itr)
        {
            if (itr->value.IsArray())
            {
                for (auto numIterator = itr->value.Begin(); numIterator != itr->value.End(); ++numIterator)
                {
                    if (numIterator->IsInt())
                    {
                        int cat = numIterator->GetInt();
                    }
                }
            }
        }*/
    }

    DirectX::XMFLOAT3 SceneLoader::getFloat3FromArray(rapidjson::GenericValue<bool, rapidjson::Value>& jsonArray)
    {
        XMFLOAT3 returnValue;
        int index = 0;
        for (auto itr = jsonArray.Begin(); itr != jsonArray.End(); ++itr)
        {
            if (index == 0) returnValue.x = itr->GetFloat();
            else if (index == 0) returnValue.y = itr->GetFloat();
            else if (index == 0) returnValue.z = itr->GetFloat();

            ++index;
        }

        return returnValue;
    }
}