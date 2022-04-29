#include "depth_of_field_post_process.h"

#include "../graphics_handler.h"
#include "../data/resource_manager.h"

namespace hrzn::gfx
{
    DepthOfFieldPostProcess::DepthOfFieldPostProcess(UINT width, UINT height, GeometryBuffer* geometryBuffer) :
        GBufferPostProcess(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, geometryBuffer),
        m_gaussianBlurA(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 0.8f),
        m_gaussianBlurB(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 0.8f)
    {
    }

    DepthOfFieldPostProcess::~DepthOfFieldPostProcess()
    {
    }

    /***********************************************

    MARKING SCHEME: Recent / Advanced graphics algorithms or techniques

    DESCRIPTION: This is the post process class which runs the Depth of Field post process,
    It uses two gaussain blurs to get a more powerful blur, as iterative blurring is not possible yet

    Once these blurs have been performed it sets the final one as a shader resource, along with the
    colour image and the geometry buffer's depth stencil buffer. Then the depth of field shader is run
    while rendering to a quad. The focus depth is automatically calculated by sampling the pixel in the
    middle of the screen in the shader.

    COMMENT INDEX: 15

    ***********************************************/

    void DepthOfFieldPostProcess::run(RenderTexture* input)
    {
        ID3D11DeviceContext* deviceContext = GraphicsHandler::it().getDeviceContext();

        // Blur the screen texture
        m_gaussianBlurA.run(input);
        m_gaussianBlurB.run(m_gaussianBlurA.getResult());

        // Set the input texture as shader resource 0
        deviceContext->PSSetShaderResources(0, 1, input->m_shaderResourceView.GetAddressOf());

        // Set the blurred input texture as shader resource 1
        deviceContext->PSSetShaderResources(1, 1, m_gaussianBlurB.getResult()->m_shaderResourceView.GetAddressOf());

        // Set the gbuffer depth texture as shader resource 2
        deviceContext->PSSetShaderResources(2, 1, m_geometryBuffer->m_depthStencil.m_shaderResourceView.GetAddressOf());

        // Perform depth of field
        deviceContext->PSSetShader(ResourceManager::it().getPSPtr("misc_depth_of_field")->getShader(), NULL, 0);
        m_result.setAsRenderTargetAndDrawQuad();

        // Unset shader resources
        ID3D11ShaderResourceView* nullShaderResourceViews[3] = { NULL };
        deviceContext->PSSetShaderResources(0, 3, nullShaderResourceViews);
    }

    const char* DepthOfFieldPostProcess::getName()
    {
        return "DepthOfField";
    }
}