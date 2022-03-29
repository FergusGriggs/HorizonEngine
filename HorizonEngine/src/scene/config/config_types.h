#pragma once

#include <string>

#include "../../maths/vec3.h"

namespace hrzn::scene::config
{
    struct TimeConfig
    {
        TimeConfig() :
            m_startTime(0.335f),
            m_dayNightCycle(false),
            m_timePaused(false),
            m_dayDuration(60.0f)
        {
        }

        float m_startTime;
        bool  m_dayNightCycle;
        bool  m_timePaused;
        float m_dayDuration;
    };

    struct OceanConfig
    {
        OceanConfig() :
            m_enabled(true),

            m_waveCount(30),

            m_waveScale(6.6f),
            m_waveScaleMultiplier(0.745f),

            m_wavePeriod(21.5f),
            m_waveSpeed(12.0f),
            m_waveSeed(656.993f),

            m_foamStart(0.97f),
            m_colourChangeStart(1.88f)
        {
        }

        bool  m_enabled;

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
        CloudConfig() :
            m_enabled(true),

            m_lightAbsorbtionThroughClouds(0.084f),
            m_lightAbsorbtionTowardsSun(0.392f),
            m_phaseFactor(0.266f),
            m_darknessThreshold(0.073f),
            
            m_cloudSpeed(0.02f),
            m_cloudCoverage(0.497f),
            m_cloudHeight(2000.0f),
            
            m_numSteps(40),
            m_stepSize(85.0f)
        {
        }

        bool  m_enabled;

        float m_lightAbsorbtionThroughClouds;
        float m_lightAbsorbtionTowardsSun;
        float m_phaseFactor;
        float m_darknessThreshold;

        float m_cloudSpeed;
        float m_cloudCoverage;
        float m_cloudHeight;

        int   m_numSteps;
        float m_stepSize;
    };

    struct AtmosphereConfig
    {
        AtmosphereConfig() :
            m_sunSize(75.0f),

            m_density(0.65f),
            m_multiScatterPhase(0.27f),
            m_anisotropicIntensity(1.0f),
            m_zenithOffset(-0.06f),

            m_nightDensity(1.2f),
            m_nightZenithYClamp(0.1f)
        {
        }

        float  m_sunSize;

        float  m_density;
        float  m_multiScatterPhase;
        float  m_anisotropicIntensity;
        float  m_zenithOffset;

        float  m_nightDensity;
        float  m_nightZenithYClamp;
    };

    struct TerrainConfig
    {
        enum class DimensionType
        {
            eUnset,
            e2D,
            e3D
        };

        enum class GenerationType2D
        {
            eHeightmap,
            eDiamondSquare,
            eFaultLine,
            eCircle
        };

        enum class GenerationType3D
        {
            eDefault
        };

        TerrainConfig() :
            m_enabled(true),
            m_dimensionType(DimensionType::e2D),
            m_isInfinite(false),
            m_chunkScale(128.0f),
            m_originPosition(maths::Vec3f(0.0f)),
            
            m_generationType2D(GenerationType2D::eHeightmap),
            m_heightmapPath("res/textures/terrain/coastMountain513.raw"),

            m_diamondSquareResolution(),
            
            m_dimensions(128, 128),
            m_interations(1000),
            
            m_minRadius(2),
            m_maxRadius(6),

            m_generationType3D(GenerationType3D::eDefault)
        {
        }

        // Always loaded
        bool          m_enabled;
        DimensionType m_dimensionType;
        bool          m_isInfinite;
        float         m_chunkScale;
        maths::Vec3f  m_originPosition;

        // Loaded if 2d
        GenerationType2D m_generationType2D;
        // Heightmap
        std::string      m_heightmapPath;
        // Diamond Square
        int              m_diamondSquareResolution;
        // Fault Line and Circle
        maths::Vec2i     m_dimensions;
        int              m_interations;
        // Circle
        int              m_minRadius;
        int              m_maxRadius;

        // Loaded if 3d
        GenerationType3D m_generationType3D;
    };
}
