#pragma once

#include "post_process.h"

namespace hrzn::gfx
{
    class ChromaticAberrationPostProcess : public PostProcess
    {
    public:
        ChromaticAberrationPostProcess(UINT width, UINT height);
        virtual ~ChromaticAberrationPostProcess();

        virtual void run(RenderTexture* input) override;
        virtual const char* getName() override;
    };
}
