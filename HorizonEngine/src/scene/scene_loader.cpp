#include "scene_loader.h"

#include <fstream>
#include <sstream>
#include <algorithm>

#include "../utils/error_logger.h"

namespace hrzn::scene
{
    SceneLoader::SceneLoader()
    {
    }

    SceneLoader::~SceneLoader()
    {
    }

    void SceneLoader::loadScene(std::string sceneName)
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

            std::string finalErrorString = "Failed to load JSON\nCode: '" + std::to_string(error) + "'\nLine Num: '" + std::to_string(lineNumber) + "'\nError String: '" + errorString + "'\n";

            utils::ErrorLogger::log(finalErrorString);
        }

        for (auto itr = m_document.MemberBegin(); itr != m_document.MemberEnd(); ++itr)
        {
            if (itr->value.IsString())
            {
                int cat = 12;
            }

            if (itr->value.IsBool())
            {
                int cat = 12;
            }

            if (itr->value.IsNull())
            {
                int cat = 12;
            }

            if (itr->value.IsInt())
            {
                int cat = 12;
            }

            if (itr->value.IsFloat())
            {
                int cat = 12;
            }

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
        }
    }
}