#include "gbuffer_post_process.h"

namespace hrzn::gfx
{
    GBufferPostProcess::GBufferPostProcess(UINT width, UINT height, GeometryBuffer* geometryBuffer) :
        PostProcess(width, height),
        m_geometryBuffer(geometryBuffer)
    {
    }

    GBufferPostProcess::~GBufferPostProcess()
    {
    }

    void GBufferPostProcess::run(RenderTexture* input)
    {
    }
}