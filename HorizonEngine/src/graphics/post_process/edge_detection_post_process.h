#pragma once

#include "gbuffer_post_process.h"
#include "gaussian_blur_post_process.h"

namespace hrzn::gfx
{
    class EdgeDetectionPostProcess : public GBufferPostProcess
    {
    public:
        EdgeDetectionPostProcess(UINT width, UINT height, GeometryBuffer* geometryBuffer);
        virtual ~EdgeDetectionPostProcess();

        virtual void run(RenderTexture* input) override;
        virtual const char* getName() override;
    };
}
