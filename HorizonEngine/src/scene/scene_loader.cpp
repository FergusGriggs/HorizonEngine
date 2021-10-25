#include "scene_loader.h"

#include <fstream>

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
        //std::string sceneFilePath = "res/scenes/";
        //sceneFilePath += sceneName;

        //std::ifstream sceneFile(sceneFilePath.c_str());

        const char json[] = " { \"hello\" : \"world\", \"t\" : true , \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4] } ";

        rapidjson::StringStream ss(json);
        m_reader.Parse(ss, m_handler);
    }
}