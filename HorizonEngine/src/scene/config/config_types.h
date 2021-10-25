#pragma once

namespace hrzn::scene::config
{
    struct MetaConfig
    {
        float m_startTime;
        bool  m_dayNightCycle;
        bool  m_timePaused;
        float m_dayDuration;
    };

    struct OceanConfig
    {
        int   m_waveCount;

        float m_waveScale;
        float m_waveScaleMultiplier;

        float m_wavePeriod;
        float m_waveSpeed;
        float m_waveSeed;
        
        float m_foamStart;
        float m_colourChangeStart;
    };

    struct CloudConfig
    {
        float m_lightAbsorbtionThroughClouds;
        float m_lightAbsorbtionTowardsSun;
        float m_phaseFactor;
        float m_darknessThreshold;

        float m_cloudSpeed;
        float m_cloudHeight;

        int   m_numSteps;
        float m_stepSize;
    };

    struct AtmosphereConfig
    {
        float  m_sunSize;

        float  m_density;
        float  m_multiScatterPhase;
        float  m_anisotropicIntensity;
        float  m_zenithOffset;

        float  m_nightDensity;
        float  m_nightZenithYClamp;
    };
}
