#include "grayscale_post_process.h"

#include "../graphics_handler.h"
#include "../data/resource_manager.h"

namespace hrzn::gfx
{
    GrayscalePostProcess::GrayscalePostProcess(UINT width, UINT height) :
        PostProcess(DXGI_FORMAT_R8G8B8A8_UNORM, width, height)
    {
    }

    GrayscalePostProcess::~GrayscalePostProcess()
    {
    }

    void GrayscalePostProcess::run(RenderTexture* input)
    {
        ID3D11DeviceContext* deviceContext = GraphicsHandler::it().getDeviceContext();

        // Set input as shader resource
        deviceContext->PSSetShaderResources(0, 1, input->m_shaderResourceView.GetAddressOf());

        // Run grayscale shader
        deviceContext->PSSetShader(ResourceManager::it().getPSPtr("misc_grayscale")->getShader(), NULL, 0);
        m_result.setAsRenderTargetAndDrawQuad();

        // Unset input as shader resource
        ID3D11ShaderResourceView* const nullShaderResourceViews[1] = { NULL };
        deviceContext->PSSetShaderResources(0, 1, nullShaderResourceViews);
    }

    const char* GrayscalePostProcess::getName()
    {
        return "GrayScale";
    }
}