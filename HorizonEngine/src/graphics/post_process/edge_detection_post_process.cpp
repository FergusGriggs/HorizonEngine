#include "edge_detection_post_process.h"

#include "../graphics_handler.h"
#include "../data/resource_manager.h"

namespace hrzn::gfx
{
    EdgeDetectionPostProcess::EdgeDetectionPostProcess(UINT width, UINT height, GeometryBuffer* geometryBuffer) :
        GBufferPostProcess(width, height, geometryBuffer)
    {
    }

    EdgeDetectionPostProcess::~EdgeDetectionPostProcess()
    {
    }

    void EdgeDetectionPostProcess::run(RenderTexture* input)
    {
        ID3D11DeviceContext* deviceContext = GraphicsHandler::it().getDeviceContext();

        // Set the input texture as shader resource 0
        deviceContext->PSSetShaderResources(0, 1, input->m_shaderResourceView.GetAddressOf());

        // Set the gbuffer depth texture as shader resource 1
        deviceContext->PSSetShaderResources(1, 1, m_geometryBuffer->m_depthStencil.m_shaderResourceView.GetAddressOf());

        // Set the gbuffer normal texture as shader resource 1
        deviceContext->PSSetShaderResources(2, 1, m_geometryBuffer->m_normalAO.m_shaderResourceView.GetAddressOf());

        // Perform depth of field
        deviceContext->PSSetShader(ResourceManager::it().getPSPtr("edge_detection")->getShader(), NULL, 0);
        m_result.setAsRenderTargetAndDrawQuad();

        // Unset shader resources
        ID3D11ShaderResourceView* nullShaderResourceViews[3] = { NULL };
        deviceContext->PSSetShaderResources(0, 3, nullShaderResourceViews);
    }

    const char* EdgeDetectionPostProcess::getName()
    {
        return "EdgeDetection";
    }
}