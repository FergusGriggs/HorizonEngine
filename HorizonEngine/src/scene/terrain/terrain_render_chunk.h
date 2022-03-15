#pragma once

#include <vector>

#include "../../graphics/data/mesh.h"

namespace hrzn::scene::terrain
{
    class RenderChunk
    {
    public:
        RenderChunk();
        ~RenderChunk();

        std::vector<gfx::Mesh*>& GetMeshes();

    private:
        std::vector<gfx::Mesh*> m_meshes;
    };
}
