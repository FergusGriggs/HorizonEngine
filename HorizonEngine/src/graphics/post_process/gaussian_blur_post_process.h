#pragma once

#include "post_process.h"

namespace hrzn::gfx
{
    class GaussianBlurPostProcess : public PostProcess
    {
    public:
        GaussianBlurPostProcess(UINT width, UINT height, float texSizeMult);
        virtual ~GaussianBlurPostProcess();

        virtual void run(RenderTexture* input) override;
        virtual const char* getName() override;

    private:
        RenderTexture m_intermediateTexture;
    };
}
