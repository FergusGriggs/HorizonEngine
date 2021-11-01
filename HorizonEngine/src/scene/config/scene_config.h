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

        const config::MetaConfig&       getMetaConfig() const;
        config::MetaConfig&             getWritableMetaConfig();

        const config::OceanConfig&      getOceanConfig() const;
        config::OceanConfig&            getWritableOceanConfig();

        const config::CloudConfig&      getCloudConfig() const;
        config::CloudConfig&            getWritableCloudConfig();

        const config::AtmosphereConfig& getAtmosphereConfig() const;
        config::AtmosphereConfig&       getWritableAtmosphereConfig();

    private:
        config::MetaConfig       m_metaConfig;
        config::OceanConfig      m_oceanConfig;
        config::CloudConfig      m_cloudConfig;
        config::AtmosphereConfig m_atmosphereConfig;
    };
}
