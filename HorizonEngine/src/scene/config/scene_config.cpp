#include "scene_config.h"

namespace hrzn::scene
{
    SceneConfig::SceneConfig() :
        m_metaConfig(),
        m_oceanConfig(),
        m_cloudConfig(),
        m_atmosphereConfig()
    {
    }

    SceneConfig::~SceneConfig()
    {
    }

    void SceneConfig::loadConfigs()
    {
    }

    void SceneConfig::saveConfigs(rapidjson::Writer<rapidjson::StringBuffer>& JSONWriter)
    {

    }

    const config::MetaConfig& SceneConfig::getMetaConfig()
    {
        return m_metaConfig;
    }

    config::MetaConfig& SceneConfig::getWritableMetaConfig()
    {
        return m_metaConfig;
    }

    const config::OceanConfig& SceneConfig::getOceanConfig()
    {
        return m_oceanConfig;
    }

    config::OceanConfig& SceneConfig::getWritableOceanConfig()
    {
        return m_oceanConfig;
    }

    const config::CloudConfig& SceneConfig::getCloudConfig()
    {
        return m_cloudConfig;
    }

    config::CloudConfig& SceneConfig::getWritableCloudConfig()
    {
        return m_cloudConfig;
    }

    const config::AtmosphereConfig& SceneConfig::getAtmosphereConfig()
    {
        return m_atmosphereConfig;
    }

    config::AtmosphereConfig& SceneConfig::getWritableAtmosphereConfig()
    {
        return m_atmosphereConfig;
    }
}
