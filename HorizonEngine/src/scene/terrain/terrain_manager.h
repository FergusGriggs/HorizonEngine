#pragma once

#include <unordered_map>

#include "../../maths/vec3.h"

#include "terrain_chunk.h"

namespace hrzn::scene
{
    class TerrainManager
    {
    public:
        TerrainManager();
        ~TerrainManager();

        void SetViewerPosition(maths::Vec3f& position);

        void Update(float deltaTime);

        void LoadCloseChunks();
        void UnloadFarChunks();        

    private:
        std::unordered_map<maths::Vec3i, TerrainChunk*> m_chunks;

        maths::Vec3f                                    m_viewerPosition;
    };
}
