#pragma once

#include "post_process.h"

namespace hrzn::gfx
{
    class GrayscalePostProcess : public PostProcess
    {
    public:
        GrayscalePostProcess(UINT width, UINT height);
        virtual ~GrayscalePostProcess();

        virtual void run(RenderTexture* input) override;
        virtual const char* getName() override;
    };
}
