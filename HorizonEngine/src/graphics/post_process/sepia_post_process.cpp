#include "sepia_post_process.h"

#include "../graphics_handler.h"
#include "../data/resource_manager.h"

namespace hrzn::gfx
{
    SepiaPostProcess::SepiaPostProcess(UINT width, UINT height) :
        PostProcess(width, height)
    {
    }

    SepiaPostProcess::~SepiaPostProcess()
    {
    }

    void SepiaPostProcess::run(RenderTexture* input)
    {
        ID3D11DeviceContext* deviceContext = GraphicsHandler::it().getDeviceContext();

        // Set input as shader resource
        deviceContext->PSSetShaderResources(0, 1, input->m_shaderResourceView.GetAddressOf());

        // Run grayscale shader
        deviceContext->PSSetShader(ResourceManager::it().getPSPtr("sepia")->getShader(), NULL, 0);
        m_result.setAsRenderTargetAndDrawQuad();

        // Unset input as shader resource
        ID3D11ShaderResourceView* const nullShaderResourceViews[1] = { NULL };
        deviceContext->PSSetShaderResources(0, 1, nullShaderResourceViews);
    }

    const char* SepiaPostProcess::getName()
    {
        return "Sepia";
    }
}