#include "bloom_post_process.h"

#include "../graphics_handler.h"
#include "../data/resource_manager.h"

namespace hrzn::gfx
{
    BloomPostProcess::BloomPostProcess(UINT width, UINT height, GeometryBuffer* geometryBuffer) :
        GBufferPostProcess(width, height, geometryBuffer),
        m_gaussianBlur(width, height, 0.35f)
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
        deviceContext->PSSetShaderResources(0, 1, m_geometryBuffer->m_emissionMetallic.m_shaderResourceView.GetAddressOf());

        // Iscolate the emission
        deviceContext->PSSetShader(ResourceManager::it().getPSPtr("quad_iscolate_emission")->getShader(), NULL, 0);
        m_iscolatedEmissionTexture.setAsRenderTargetAndDrawQuad();

        // Unset self as shader resource
        ID3D11ShaderResourceView* const nullShaderResourceViews[1] = { NULL };
        deviceContext->PSSetShaderResources(0, 1, nullShaderResourceViews);

        // Blur the iscolated emission texture
        m_gaussianBlur.run(&m_iscolatedEmissionTexture);

        // Set the original render as shader resource 0
        deviceContext->PSSetShaderResources(0, 1, input->m_shaderResourceView.GetAddressOf());

        // Set the blurred emission as shader resource 1
        deviceContext->PSSetShaderResources(1, 1, m_gaussianBlur.getResult()->m_shaderResourceView.GetAddressOf());

        deviceContext->PSSetShader(ResourceManager::it().getPSPtr("bloom")->getShader(), NULL, 0);
        m_result.setAsRenderTargetAndDrawQuad();

        deviceContext->PSSetShaderResources(0, 1, nullShaderResourceViews);
    }

    const char* BloomPostProcess::getName()
    {
        return "Bloom";
    }
}