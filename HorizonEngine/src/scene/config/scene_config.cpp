#include "scene_config.h"

namespace hrzn::scene
{
    SceneConfig::SceneConfig() :
        m_timeConfig(),
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

    void SceneConfig::saveConfigs(rapidjson::Writer<rapidjson::StringBuffer>& JSONWriter) const
    {

    }

    const config::TimeConfig& SceneConfig::getTimeConfig() const
    {
        return m_timeConfig;
    }

    config::TimeConfig& SceneConfig::getWritableTimeConfig()
    {
        return m_timeConfig;
    }

    void SceneConfig::setTimeConfig(const config::TimeConfig& timeConfig)
    {
        m_timeConfig = timeConfig;
    }

    const config::OceanConfig& SceneConfig::getOceanConfig() const
    {
        return m_oceanConfig;
    }

    config::OceanConfig& SceneConfig::getWritableOceanConfig()
    {
        return m_oceanConfig;
    }

    void SceneConfig::setOceanConfig(const config::OceanConfig& oceanConfig)
    {
        m_oceanConfig = oceanConfig;
    }

    const config::CloudConfig& SceneConfig::getCloudConfig() const
    {
        return m_cloudConfig;
    }

    config::CloudConfig& SceneConfig::getWritableCloudConfig()
    {
        return m_cloudConfig;
    }

    void SceneConfig::setCloudConfig(const config::CloudConfig& cloudConfig)
    {
        m_cloudConfig = cloudConfig;
    }

    const config::AtmosphereConfig& SceneConfig::getAtmosphereConfig() const
    {
        return m_atmosphereConfig;
    }

    config::AtmosphereConfig& SceneConfig::getWritableAtmosphereConfig()
    {
        return m_atmosphereConfig;
    }

    void SceneConfig::setAtmosphereConfig(const config::AtmosphereConfig& atmosphereConfig)
    {
        m_atmosphereConfig = atmosphereConfig;
    }

    const config::TerrainConfig& SceneConfig::getTerrainConfig() const
    {
        return m_terrainConfig;
    }

    config::TerrainConfig& SceneConfig::getWritableTerrainConfig()
    {
        return m_terrainConfig;
    }

    void SceneConfig::setTerrainConfig(const config::TerrainConfig& terrainConfig)
    {
        m_terrainConfig = terrainConfig;
    }
}
