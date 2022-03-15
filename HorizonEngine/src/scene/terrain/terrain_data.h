#pragma once

#include "../../maths/vec2.h"
#include "../../maths/vec3.h"

namespace hrzn::scene::terrain
{
    static const int sc_chunkSize = 32;

    class DataChunk2D
    {
    public:
        DataChunk2D();
        ~DataChunk2D();

        void SetValue();

    private:
        maths::Vec2i m_chunkPos;
        float        m_data[sc_chunkSize][sc_chunkSize];
    };

    class DataChunk3D
    {
    public:
        DataChunk3D();
        ~DataChunk3D();

    private:
        maths::Vec3i m_chunkPos;
        float        m_data[sc_chunkSize][sc_chunkSize][sc_chunkSize];
    };
}
