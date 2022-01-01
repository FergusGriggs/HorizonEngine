#pragma once

#include "../data/render_textures.h"

namespace hrzn::gfx
{
    class PostProcess
    {
    protected:
        PostProcess(UINT width, UINT height);

    public:
        virtual ~PostProcess();

        virtual void run(RenderTexture* input);

        virtual const char* getName();

        RenderTexture* getResult();

    protected:
        RenderTexture m_result;
    };
}
