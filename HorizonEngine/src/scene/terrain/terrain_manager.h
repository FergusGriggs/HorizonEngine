#pragma once

#include <unordered_map>

#include "../../maths/vec3.h"

#include "terrain_chunk.h"

#include "dual_contouring/octree.h"

#include "../../graphics/data/mesh.h"

namespace hrzn::scene
{
    class TerrainManager
    {
    public:
        static TerrainManager& it();

        ~TerrainManager();

        void SetViewerPosition(maths::Vec3f& position);

        void Update(float deltaTime);

        gfx::Mesh* GetTestMesh();

        void LoadCloseChunks();
        void UnloadFarChunks();        

    private:
        TerrainManager();

        //std::unordered_map<maths::Vec3i, TerrainChunk*> m_chunks;
        terrain::OctreeNode*                              m_testOctree;
        gfx::Mesh*                                        m_mesh;

        maths::Vec3f                                      m_viewerPosition;
    };
}
