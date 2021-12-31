#pragma once

#include "post_process.h"

#include "../buffers/geometry_buffer.h"

namespace hrzn::gfx
{
    class GBufferPostProcess : public PostProcess
    {
    public:
        GBufferPostProcess(UINT width, UINT height, GeometryBuffer* geometryBuffer);
        virtual ~GBufferPostProcess();

        virtual void run(RenderTexture* input) override;

    protected:
        GeometryBuffer* m_geometryBuffer;
    };
}

