#include "chromatic_aberration_post_process.h"

#include "../graphics_handler.h"
#include "../data/resource_manager.h"

namespace hrzn::gfx
{
    ChromaticAberrationPostProcess::ChromaticAberrationPostProcess(UINT width, UINT height) :
        PostProcess(width, height)
    {
    }

    ChromaticAberrationPostProcess::~ChromaticAberrationPostProcess()
    {
    }

    void ChromaticAberrationPostProcess::run(RenderTexture* input)
    {
        ID3D11DeviceContext* deviceContext = GraphicsHandler::it().getDeviceContext();

        // Set input as shader resource
        deviceContext->PSSetShaderResources(0, 1, input->m_shaderResourceView.GetAddressOf());

        // Run grayscale shader
        deviceContext->PSSetShader(ResourceManager::it().getPSPtr("chromatic_abberation")->getShader(), NULL, 0);
        m_result.setAsRenderTargetAndDrawQuad();

        // Unset input as shader resource
        ID3D11ShaderResourceView* const nullShaderResourceViews[1] = { NULL };
        deviceContext->PSSetShaderResources(0, 1, nullShaderResourceViews);
    }

    const char* ChromaticAberrationPostProcess::getName()
    {
        return "ChromaticAberration";
    }
}