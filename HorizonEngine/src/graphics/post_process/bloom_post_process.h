#pragma once

#include "gbuffer_post_process.h"
#include "gaussian_blur_post_process.h"

namespace hrzn::gfx
{
    class BloomPostProcess : public GBufferPostProcess
    {
    public:
        BloomPostProcess(UINT width, UINT height, GeometryBuffer* geometryBuffer);
        virtual ~BloomPostProcess();

        virtual void run(RenderTexture* input) override;
        virtual const char* getName() override;

    private:
        RenderTexture           m_iscolatedEmissionTexture;
        GaussianBlurPostProcess m_gaussianBlur;
    };
}
