#include "terrain_manager.h"

#include "../../graphics/data/vertex.h"

#include "../../graphics/data/resource_manager.h"

#include "../scene_manager.h"

namespace hrzn::scene
{
    TerrainManager::TerrainManager() :
        m_staticTerrainMesh(nullptr),
        m_chunkScale(128.0f),
        m_originPosition(maths::Vec3f(75.0f, -7.5f, 0.0f))
    {
        config::TerrainConfig terrainConfig = SceneManager::it().getSceneConfig().getTerrainConfig();
        
        m_chunkScale = terrainConfig.m_chunkScale;
        m_originPosition = terrainConfig.m_originPosition;

        if (terrainConfig.m_enabled)
        {
            if (terrainConfig.m_dimensionType == config::TerrainConfig::DimensionType::e2D)
            {
                if (terrainConfig.m_generationType2D == config::TerrainConfig::GenerationType2D::eHeightmap)
                {
                    createStaticTerrainMeshFromHeightmap(terrainConfig.m_heightmapPath);
                }
                else if (terrainConfig.m_generationType2D == config::TerrainConfig::GenerationType2D::eDiamondSquare)
                {
                    createStaticTerrainMeshUsingDiamondSquare(maths::Vec3i(128, 1, 128));
                }
                else if (terrainConfig.m_generationType2D == config::TerrainConfig::GenerationType2D::eFaultLine)
                {
                    createStaticTerrainMeshUsingFaultLine(maths::Vec3i(128, 1, 128));
                }
                else if (terrainConfig.m_generationType2D == config::TerrainConfig::GenerationType2D::eCircle)
                {
                    createStaticTerrainMeshUsingCircle(maths::Vec3i(128, 1, 128));
                }
            }
            else
            {
                createDualContouringStaticMesh();
            }
        }
    }

    TerrainManager& TerrainManager::it()
    {
        static TerrainManager s;
        return s;
    }

    TerrainManager::~TerrainManager()
    {
    }

    void TerrainManager::setViewerPosition(maths::Vec3f& position)
    {
    }

    void TerrainManager::update(float deltaTime)
    {
    }

    void TerrainManager::loadCloseChunks()
    {
    }

    void TerrainManager::unloadFarChunks()
    {
    }

    void TerrainManager::renderTerrain(bool useGBuffer, bool bindPSData)
    {
        m_staticTerrainMesh->draw(useGBuffer, bindPSData);
    }

    void TerrainManager::initialiseStaticTerrainHeights(const maths::Vec3i& dimensions)
    {
        m_staticTerrainDimensions = dimensions;
        m_staticTerrainHeights = new float* [dimensions.x];

        for (int i = 0; i < dimensions.x; ++i)
        {
            m_staticTerrainHeights[i] = new float[dimensions.z];

            for (int j = 0; j < dimensions.z; ++j)
            {
                m_staticTerrainHeights[i][j] = 0.0f;
            }
        }
    }

    void TerrainManager::createStaticTerrainMeshFromHeightmap(const std::string& heightmapFilePath)
    {
#define RAW_IMAGE_WIDTH 513
#define RAW_IMAGE_HEIGHT 513
        initialiseStaticTerrainHeights(maths::Vec3i(RAW_IMAGE_WIDTH, 1, RAW_IMAGE_HEIGHT));

        std::ifstream heightmapFile;
        heightmapFile.open(heightmapFilePath, std::ios::binary);

        int dataSize = RAW_IMAGE_WIDTH * RAW_IMAGE_HEIGHT;
        std::vector<unsigned char> heightData(dataSize);

        if (heightmapFile)
        {
            heightmapFile.read((char*)&heightData[0], (std::streamsize)dataSize);

            heightmapFile.close();
        }

        int rowIndex = 0;
        int columnIndex = 0;
        for (int i = 0; i < dataSize; ++i)
        {
            m_staticTerrainHeights[rowIndex][columnIndex] = (float)heightData[i] / 1024.0f;

            ++columnIndex;
            if (columnIndex == RAW_IMAGE_WIDTH)
            {
                columnIndex = 0;
                ++rowIndex;
            }
        }

        createStaticTerrainMeshUsingHeights();
    }

    void TerrainManager::createStaticTerrainMeshUsingDiamondSquare(const maths::Vec3i& dimensions)
    {
        initialiseStaticTerrainHeights(dimensions);

        createStaticTerrainMeshUsingHeights();
    }

    void TerrainManager::createStaticTerrainMeshUsingCircle(const maths::Vec3i& dimensions)
    {
        initialiseStaticTerrainHeights(dimensions);

        createStaticTerrainMeshUsingHeights();
    }

    void TerrainManager::createStaticTerrainMeshUsingFaultLine(const maths::Vec3i& dimensions)
    {
        initialiseStaticTerrainHeights(dimensions);

        createStaticTerrainMeshUsingHeights();
    }

    void TerrainManager::createStaticTerrainMeshUsingHeights()
    {
        std::vector<gfx::Vertex> vertices;
        std::vector<DWORD> indices;

        maths::Vec3f vertStep = (m_staticTerrainDimensions - maths::Vec3i(1, 0, 1)).createVec3f().reciprocal() * m_chunkScale;

        std::vector<maths::Vec3f> vertexPositions;
        // Place a vertex at every height
        for (int i = 0; i < m_staticTerrainDimensions.x; ++i)
        {
            for (int j = 0; j < m_staticTerrainDimensions.z; ++j)
            {
                maths::Vec3f vertPosition = m_originPosition - m_chunkScale * 0.5f + maths::Vec3f(vertStep.x * (float)i, 0.0f, 0.0f) + maths::Vec3f(0.0f, 0.0f, vertStep.z * (float)j) + maths::Vec3f(0.0f, m_chunkScale * 0.5f + m_staticTerrainHeights[i][j] * m_chunkScale, 0.0f);

                vertexPositions.push_back(vertPosition);
            }
        }

        // Create flat shaded faces
        for (int i = 0; i < m_staticTerrainDimensions.x - 1; ++i)
        {
            for (int j = 0; j < m_staticTerrainDimensions.z - 1; ++j)
            {
                DWORD currentIndex = vertices.size();

                maths::Vec3f topLeft, topRight, botLeft, botRight;

                topLeft = vertexPositions[j * m_staticTerrainDimensions.z + i];
                topRight = vertexPositions[j * m_staticTerrainDimensions.z + (i + 1)];
                botLeft = vertexPositions[(j + 1) * m_staticTerrainDimensions.z + i];
                botRight = vertexPositions[(j + 1) * m_staticTerrainDimensions.z + (i + 1)];

                maths::Vec3f face1Normal = maths::Vec3f::normalise(maths::Vec3f::cross(topRight - topLeft, botLeft - topLeft));
                maths::Vec3f face2Normal = maths::Vec3f::normalise(maths::Vec3f::cross(topRight - botLeft, botRight - botLeft));

                // Add vertices to the list
                vertices.emplace_back(topLeft.x, topLeft.y, topLeft.z, face1Normal.x, face1Normal.y, face1Normal.z, 0.0f, 0.0f);
                vertices.emplace_back(topRight.x, topRight.y, topRight.z, face1Normal.x, face1Normal.y, face1Normal.z, 0.0f, 0.0f);
                vertices.emplace_back(botLeft.x, botLeft.y, botLeft.z, face1Normal.x, face1Normal.y, face1Normal.z, 0.0f, 0.0f);

                vertices.emplace_back(botLeft.x, botLeft.y, botLeft.z, face2Normal.x, face2Normal.y, face2Normal.z, 0.0f, 0.0f);
                vertices.emplace_back(topRight.x, topRight.y, topRight.z, face2Normal.x, face2Normal.y, face2Normal.z, 0.0f, 0.0f);
                vertices.emplace_back(botRight.x, botRight.y, botRight.z, face2Normal.x, face2Normal.y, face2Normal.z, 0.0f, 0.0f);

                // Add indices to the list
                indices.push_back(currentIndex);
                indices.push_back(currentIndex + 1);
                indices.push_back(currentIndex + 2);

                indices.push_back(currentIndex + 3);
                indices.push_back(currentIndex + 4);
                indices.push_back(currentIndex + 5);
            }
        }

        // Create mesh
        m_staticTerrainMesh = new gfx::Mesh(vertices, indices, gfx::ResourceManager::it().getMaterialPtr("terrain"), XMMatrixIdentity());
    }

    gfx::Mesh* TerrainManager::getStaticTerrainMesh()
    {
        return m_staticTerrainMesh;
    }

    void TerrainManager::createDualContouringStaticMesh()
    {
        // octreeSize must be a power of two!
        const int octreeSize = 128;

        const int MAX_THRESHOLDS = 5;
        const float THRESHOLDS[MAX_THRESHOLDS] = { -1.f, 0.1f, 1.f, 10.f, 50.f };
        int thresholdIndex = 0;

        std::vector<gfx::Vertex> vertices;
        std::vector<DWORD> indices;

        m_testOctree = terrain::BuildOctree(maths::Vec3i(-octreeSize / 2), octreeSize, THRESHOLDS[thresholdIndex]);
        terrain::GenerateMeshFromOctree(m_testOctree, vertices, indices);

        m_staticTerrainMesh = new gfx::Mesh(vertices, indices, gfx::ResourceManager::it().getMaterialPtr("terrain"), XMMatrixIdentity());
    }
}
