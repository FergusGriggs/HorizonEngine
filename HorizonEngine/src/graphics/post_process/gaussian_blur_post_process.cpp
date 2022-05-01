#include "gaussian_blur_post_process.h"

#include "../graphics_handler.h"
#include "../data/resource_manager.h"

namespace hrzn::gfx
{
    GaussianBlurPostProcess::GaussianBlurPostProcess(DXGI_FORMAT format, UINT width, UINT height, float texSizeMult) :
        PostProcess(format, (UINT)(width * 0.5f * texSizeMult), (UINT)(height * 0.5f * texSizeMult))
    {
        m_intermediateTexture.initialise(format, (UINT)(width * texSizeMult), (UINT)(height * texSizeMult));
    }

    GaussianBlurPostProcess::~GaussianBlurPostProcess()
    {
    }

    void GaussianBlurPostProcess::run(RenderTexture* input)
    {
        ID3D11DeviceContext* deviceContext = GraphicsHandler::it().getDeviceContext();

        // Set input texture as shader resource
        deviceContext->PSSetShaderResources(0, 1, input->m_shaderResourceView.GetAddressOf());

        // Perform the horizontal blur
        deviceContext->PSSetShader(ResourceManager::it().getPSPtr("misc_gaussian_blur_horizontal")->getShader(), NULL, 0);
        m_intermediateTexture.setAsRenderTargetAndDrawQuad();

        // Set intermediate texture as shader resource
        deviceContext->PSSetShaderResources(0, 1, m_intermediateTexture.m_shaderResourceView.GetAddressOf());

        // Perform the vertical blur
        deviceContext->PSSetShader(ResourceManager::it().getPSPtr("misc_gaussian_blur_vertical")->getShader(), NULL, 0);
        m_result.setAsRenderTargetAndDrawQuad();

        // Unset intermediate texture as shader resource
        ID3D11ShaderResourceView* const nullShaderResourceViews[1] = { NULL };
        deviceContext->PSSetShaderResources(0, 1, nullShaderResourceViews);
    }

    const char* GaussianBlurPostProcess::getName()
    {
        return "GuassianBlur";
    }
}