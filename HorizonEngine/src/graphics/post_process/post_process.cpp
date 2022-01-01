#include "post_process.h"

namespace hrzn::gfx
{
    PostProcess::PostProcess(UINT width, UINT height)
    {
        m_result.initialise(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
    }

    PostProcess::~PostProcess()
    {
    }

    void PostProcess::run(RenderTexture* input)
    {
    }

    const char* PostProcess::getName()
    {
        return "Unnamed Post Process";
    }

    RenderTexture* PostProcess::getResult()
    {
        return &m_result;
    }
}