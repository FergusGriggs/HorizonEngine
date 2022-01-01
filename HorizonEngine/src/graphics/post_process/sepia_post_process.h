#pragma once

#include "post_process.h"

namespace hrzn::gfx
{
    class SepiaPostProcess : public PostProcess
    {
    public:
        SepiaPostProcess(UINT width, UINT height);
        virtual ~SepiaPostProcess();

        virtual void run(RenderTexture* input) override;
        virtual const char* getName() override;
    };
}
