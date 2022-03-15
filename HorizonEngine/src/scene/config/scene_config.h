#pragma once

#include "config_types.h"

#include "rapidjson/writer.h"

namespace hrzn::scene
{
    class SceneConfig
    {
    public:
        SceneConfig();
        ~SceneConfig();

        void loadConfigs();
        void saveConfigs(rapidjson::Writer<rapidjson::StringBuffer>& JSONWriter) const;

        const config::TimeConfig&       getTimeConfig() const;
        config::TimeConfig&             getWritableTimeConfig();
        void                            setTimeConfig(const config::TimeConfig& timeConfig);

        const config::OceanConfig&      getOceanConfig() const;
        config::OceanConfig&            getWritableOceanConfig();
        void                            setOceanConfig(const config::OceanConfig& oceanConfig);

        const config::CloudConfig&      getCloudConfig() const;
        config::CloudConfig&            getWritableCloudConfig();
        void                            setCloudConfig(const config::CloudConfig& cloudConfig);

        const config::AtmosphereConfig& getAtmosphereConfig() const;
        config::AtmosphereConfig&       getWritableAtmosphereConfig();
        void                            setAtmosphereConfig(const config::AtmosphereConfig& atmosphereConfig);

        const config::TerrainConfig&    getTerrainConfig() const;
        config::TerrainConfig&          getWritableTerrainConfig();
        void                            setTerrainConfig(const config::TerrainConfig& terrainConfig);

    private:
        config::TimeConfig       m_timeConfig;
        config::OceanConfig      m_oceanConfig;
        config::CloudConfig      m_cloudConfig;
        config::AtmosphereConfig m_atmosphereConfig;
        config::TerrainConfig    m_terrainConfig;
    };
}
