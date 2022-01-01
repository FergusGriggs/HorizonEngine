#pragma once

#include <string>

namespace hrzn::scene
{
    struct SceneMeta
    {
        std::string m_sceneName;
        std::string m_authorName;
        std::string m_authorEmail;
        std::string m_dateCreated;
        std::string m_dateModified;
    };
}