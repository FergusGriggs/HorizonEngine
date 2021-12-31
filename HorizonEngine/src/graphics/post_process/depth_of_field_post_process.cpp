#include "depth_of_field_post_process.h"

#include "../graphics_handler.h"
#include "../data/resource_manager.h"

namespace hrzn::gfx
{
    DepthOfFieldPostProcess::DepthOfFieldPostProcess(UINT width, UINT height, GeometryBuffer* geometryBuffer) :
        GBufferPostProcess(width, height, geometryBuffer),
        m_gaussianBlur(width, height)
    {
    }

    DepthOfFieldPostProcess::~DepthOfFieldPostProcess()
    {
    }

    void DepthOfFieldPostProcess::run(RenderTexture* input)
    {
        ID3D11DeviceContext* deviceContext = GraphicsHandler::it().getDeviceContext();

        // Blur the screen texture
        m_gaussianBlur.run(input);

        // Set the input texture as shader resource 0
        deviceContext->PSSetShaderResources(0, 1, input->m_shaderResourceView.GetAddressOf());

        // Set the blurred input texture as shader resource 1
        deviceContext->PSSetShaderResources(1, 1, m_gaussianBlur.getResult()->m_shaderResourceView.GetAddressOf());

        // Set the gbuffer depth texture as shader resource 2
        deviceContext->PSSetShaderResources(2, 1, m_geometryBuffer->m_depthStencil.m_shaderResourceView.GetAddressOf());

        // Perform depth of field
        deviceContext->PSSetShader(ResourceManager::it().getPSPtr("depth_of_field")->getShader(), NULL, 0);
        m_result.setAsRenderTargetAndDrawQuad();

        // Unset shader resources
        ID3D11ShaderResourceView* nullShaderResourceViews[3] = { NULL };
        deviceContext->PSSetShaderResources(0, 3, nullShaderResourceViews);
    }
}