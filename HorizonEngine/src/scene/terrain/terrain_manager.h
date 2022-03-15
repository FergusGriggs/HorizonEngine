#pragma once

#include <unordered_map>

#include "../../maths/vec3.h"

#include "terrain_data.h"

#include "dual_contouring/octree.h"

#include "../../graphics/data/mesh.h"

namespace hrzn::scene
{
    namespace utils
    {
        struct HashFunctionStruct
        {
            template <typename T>
            std::size_t operator()(T t) const
            {
                return std::hash(t);
            }
        };
    }

    class TerrainManager
    {
    public:
        static TerrainManager& it();

        ~TerrainManager();

        void setChunkScale(float chunkScale);
        void setOriginPosition(const maths::Vec3f& originPosition);

        void setViewerPosition(maths::Vec3f& position);
        void update(float deltaTime);
        void loadCloseChunks();
        void unloadFarChunks();

        gfx::Mesh* getStaticTerrainMesh();
        void       renderTerrain(bool useGBuffer, bool bindPSData);

        // 2D static mesh gen methods
        void initialiseStaticTerrainHeights(const maths::Vec3i& dimensions);

        void createStaticTerrainMeshFromHeightmap(const std::string& heightmapFilePath);
        void createStaticTerrainMeshUsingDiamondSquare(int resolution);
        void createStaticTerrainMeshUsingCircle(const maths::Vec3i& dimensions);
        void createStaticTerrainMeshUsingFaultLine(const maths::Vec3i& dimensions);

        void createStaticTerrainMeshUsingHeights();

        // 2D static mesh gen methods
        void createDualContouringStaticMesh();

    private:
        TerrainManager();

        // General vars
        maths::Vec3i         m_staticTerrainDimensions;
        float**              m_staticTerrainHeights;
        gfx::Mesh*           m_staticTerrainMesh;

        float                m_chunkScale;
        maths::Vec3f         m_originPosition;

        std::unordered_map<maths::Vec2i, terrain::DataChunk2D*, utils::HashFunctionStruct> m_chunkData2D;
        std::unordered_map<maths::Vec3i, terrain::DataChunk3D*, utils::HashFunctionStruct> m_chunkData3D;

        maths::Vec3f         m_viewerPosition;

        // Type specific
        terrain::OctreeNode* m_testOctree;
    };
}
