#include "post_process.h"

namespace hrzn::gfx
{
    PostProcess::PostProcess(DXGI_FORMAT format, UINT width, UINT height)
    {
        m_result.initialise(format, width, height);
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