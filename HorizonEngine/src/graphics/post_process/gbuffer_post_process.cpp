#include "gbuffer_post_process.h"

namespace hrzn::gfx
{
    GBufferPostProcess::GBufferPostProcess(DXGI_FORMAT format, UINT width, UINT height, GeometryBuffer* geometryBuffer) :
        PostProcess(format, width, height),
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