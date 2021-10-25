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
        void saveConfigs(rapidjson::Writer<rapidjson::StringBuffer>& JSONWriter);

        const config::MetaConfig&       getMetaConfig();
        config::MetaConfig&             getWritableMetaConfig();

        const config::OceanConfig&      getOceanConfig();
        config::OceanConfig&            getWritableOceanConfig();

        const config::CloudConfig&      getCloudConfig();
        config::CloudConfig&            getWritableCloudConfig();

        const config::AtmosphereConfig& getAtmosphereConfig();
        config::AtmosphereConfig&       getWritableAtmosphereConfig();

    private:
        config::MetaConfig       m_metaConfig;
        config::OceanConfig      m_oceanConfig;
        config::CloudConfig      m_cloudConfig;
        config::AtmosphereConfig m_atmosphereConfig;
    };
}
