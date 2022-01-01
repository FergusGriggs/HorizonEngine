#pragma once

#include "gbuffer_post_process.h"
#include "gaussian_blur_post_process.h"

namespace hrzn::gfx
{
    class DepthOfFieldPostProcess : public GBufferPostProcess
    {
    public:
        DepthOfFieldPostProcess(UINT width, UINT height, GeometryBuffer* geometryBuffer);
        virtual ~DepthOfFieldPostProcess();

        virtual void run(RenderTexture* input) override;
        virtual const char* getName() override;

    private:
        GaussianBlurPostProcess m_gaussianBlurA;
        GaussianBlurPostProcess m_gaussianBlurB;
    };
}