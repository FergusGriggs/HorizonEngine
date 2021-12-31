#pragma once

#include "post_process.h"

namespace hrzn::gfx
{
    class GaussianBlurPostProcess : public PostProcess
    {
    public:
        GaussianBlurPostProcess(UINT width, UINT height);
        virtual ~GaussianBlurPostProcess();

        virtual void run(RenderTexture* input) override;

    private:
        RenderTexture m_intermediateTexture;
    };
}
