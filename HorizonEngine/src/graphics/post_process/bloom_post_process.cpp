#include "bloom_post_process.h"

#include "../graphics_handler.h"
#include "../data/resource_manager.h"

namespace hrzn::gfx
{
    BloomPostProcess::BloomPostProcess(UINT width, UINT height, GeometryBuffer* geometryBuffer) :
        GBufferPostProcess(width, height, geometryBuffer),
        m_gaussianBlur(width, height)
    {
        m_iscolatedEmissionTexture.initialise(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
    }

    BloomPostProcess::~BloomPostProcess()
    {
    }

    void BloomPostProcess::run(RenderTexture* input)
    {
        ID3D11DeviceContext* deviceContext = GraphicsHandler::it().getDeviceContext();

        // Set input as shader resource
        deviceContext->PSSetShaderResources(0, 1, input->m_shaderResourceView.GetAddressOf());

        // Iscolate the emission
        deviceContext->PSSetShader(ResourceManager::it().getPSPtr("iscolate_emission")->getShader(), NULL, 0);
        m_iscolatedEmissionTexture.setAsRenderTargetAndDrawQuad();

        // Unset self as shader resource
        ID3D11ShaderResourceView* const nullShaderResourceViews[1] = { NULL };
        deviceContext->PSSetShaderResources(0, 1, nullShaderResourceViews);

        // Blur the iscolated emission texture
        m_gaussianBlur.run(&m_iscolatedEmissionTexture);

        // Copy the input texture to the result image
        input->copyTo(&m_result);

        // Overlay the blurred emission onto the final texture
        deviceContext->PSSetShaderResources(0, 1, m_gaussianBlur.getResult()->m_shaderResourceView.GetAddressOf());

        deviceContext->PSSetShader(ResourceManager::it().getPSPtr("quad_copy")->getShader(), NULL, 0);
        m_result.setAsRenderTargetAndDrawQuad(false);

        deviceContext->PSSetShaderResources(0, 1, nullShaderResourceViews);
    }
}