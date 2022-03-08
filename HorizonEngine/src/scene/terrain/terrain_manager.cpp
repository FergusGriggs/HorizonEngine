#include "terrain_manager.h"

#include "../../graphics/data/vertex.h"

#include "../../graphics/data/resource_manager.h"

namespace hrzn::scene
{
    TerrainManager::TerrainManager():
        m_mesh(nullptr)
    {
        // octreeSize must be a power of two!
        const int octreeSize = 64;

        const int MAX_THRESHOLDS = 5;
        const float THRESHOLDS[MAX_THRESHOLDS] = { -1.f, 0.1f, 1.f, 10.f, 50.f };
        int thresholdIndex = 0;

        std::vector<gfx::Vertex> vertices;
        std::vector<DWORD> indices;

        m_testOctree = terrain::BuildOctree(maths::Vec3i(-octreeSize / 2), octreeSize, THRESHOLDS[thresholdIndex]);
        terrain::GenerateMeshFromOctree(m_testOctree, vertices, indices);

        m_mesh = new gfx::Mesh(vertices, indices, gfx::ResourceManager::it().getDefaultMaterialPtr(), XMMatrixIdentity());
    }

    TerrainManager::~TerrainManager()
    {
    }

    void TerrainManager::SetViewerPosition(maths::Vec3f& position)
    {
    }

    void TerrainManager::Update(float deltaTime)
    {
    }

    gfx::Mesh* TerrainManager::GetTestMesh()
    {
        return m_mesh;
    }

    void TerrainManager::LoadCloseChunks()
    {
    }

    void TerrainManager::UnloadFarChunks()
    {
    }
}
