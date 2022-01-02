#include "image_renderer.h"

#include "graphics_handler.h"

#include "data/resource_manager.h"

namespace hrzn::gfx
{
    ImageRenderer::ImageRenderer() :
        m_depthStencilState(nullptr),
        m_rasterizerState(nullptr),

        m_quadModel(nullptr)
    {
    }

    ImageRenderer::~ImageRenderer()
    {
        clearPostProcesses();
    }

    void ImageRenderer::initialise(CD3D11_VIEWPORT viewport, ID3D11DepthStencilState* depthStencilState,
        ID3D11RasterizerState* rasterizerState)
    {
        m_viewport = viewport;

        m_geometryBuffer.initialise(static_cast<UINT>(m_viewport.Width), static_cast<UINT>(m_viewport.Height));

        m_finalImage.initialise(DXGI_FORMAT_R8G8B8A8_UNORM, static_cast<UINT>(m_viewport.Width), static_cast<UINT>(m_viewport.Height));
        m_ambientOcclusion.initialise(DXGI_FORMAT_R8_UNORM, static_cast<UINT>(m_viewport.Width), static_cast<UINT>(m_viewport.Height));

        m_depthStencilState = depthStencilState;
        m_rasterizerState = rasterizerState;

        m_quadModel = ResourceManager::it().getModelPtr("res/models/engine/screen_quad.obj");
    }

    void ImageRenderer::setViewport(CD3D11_VIEWPORT viewport, ID3D11Device* device)
    {
        m_viewport = viewport;

        release();

        m_geometryBuffer.release();
        m_finalImage.release();
        m_ambientOcclusion.release();

        m_geometryBuffer.initialise(static_cast<UINT>(m_viewport.Width), static_cast<UINT>(m_viewport.Height));

        m_finalImage.initialise(DXGI_FORMAT_R8G8B8A8_UNORM, static_cast<UINT>(m_viewport.Width), static_cast<UINT>(m_viewport.Height));
    }

    void ImageRenderer::setRasterizerState(ID3D11RasterizerState* rasterizerState)
    {
        m_rasterizerState = rasterizerState;
    }

    void ImageRenderer::addPostProcess(PostProcess* postProcess)
    {
        m_postProcesses.push_back(postProcess);
    }

    std::vector<PostProcess*>& ImageRenderer::getPostProcesses()
    {
        return m_postProcesses;
    }

    void ImageRenderer::clearPostProcesses()
    {
        for (int postProcessIndex = 0; postProcessIndex < m_postProcesses.size(); ++postProcessIndex)
        {
            delete m_postProcesses[postProcessIndex];
        }

        m_postProcesses.clear();
    }

    void ImageRenderer::render(const DirectX::XMVECTOR& eyePos, const DirectX::XMVECTOR& eyeFacing, const DirectX::XMMATRIX& viewMatrix,
        const DirectX::XMMATRIX& projectionMatrix, float nearPlane, float farPlane)
    {
        XMFLOAT3 eyePosFloat = XMFLOAT3(0.0f, 0.0f, 0.0f);
        XMStoreFloat3(&eyePosFloat, eyePos);

        ID3D11DeviceContext* deviceContext = GraphicsHandler::it().getDeviceContext();

        // Slot 0 sanity CB bind
        GraphicsHandler::it().updateSceneShaderValues();
        deviceContext->VSSetConstantBuffers(0, 1, GraphicsHandler::it().getSceneCB().getAddressOf());
        deviceContext->PSSetConstantBuffers(0, 1, GraphicsHandler::it().getSceneCB().getAddressOf());

        // Slot 2 per-frame CB bind
        GraphicsHandler::it().updatePerFrameShaderValues();
        deviceContext->VSSetConstantBuffers(2, 1, GraphicsHandler::it().getPerFrameCB().getAddressOf());
        deviceContext->PSSetConstantBuffers(2, 1, GraphicsHandler::it().getPerFrameCB().getAddressOf());

        // Slot 4 per-pass CB bind
        GraphicsHandler::it().updatePerPassShaderValues(eyePosFloat, viewMatrix, projectionMatrix, nearPlane, farPlane);
        deviceContext->VSSetConstantBuffers(4, 1, GraphicsHandler::it().getPerPassCB().getAddressOf());
        deviceContext->PSSetConstantBuffers(4, 1, GraphicsHandler::it().getPerPassCB().getAddressOf());

        // Slot 6 per-material CB bind
        deviceContext->VSSetConstantBuffers(6, 1, GraphicsHandler::it().getPerMaterialCB().getAddressOf());
        deviceContext->PSSetConstantBuffers(6, 1, GraphicsHandler::it().getPerMaterialCB().getAddressOf());

        // Slot 8 per-object CB bind
        deviceContext->VSSetConstantBuffers(8, 1, GraphicsHandler::it().getPerObjectCB().getAddressOf());
        deviceContext->PSSetConstantBuffers(8, 1, GraphicsHandler::it().getPerObjectCB().getAddressOf());

        deviceContext->RSSetViewports(1, &m_viewport);

        deviceContext->RSSetState(m_rasterizerState);
        deviceContext->OMSetDepthStencilState(m_depthStencilState, 0);

        // Clear render target views and depth stencil view
        float backgroundColour[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        deviceContext->OMSetRenderTargets(4, m_geometryBuffer.m_renderTargetViews, m_geometryBuffer.m_depthStencil.m_depthStencilView.Get());
        for (int renderTargetIndex = 0; renderTargetIndex < 4; ++renderTargetIndex)
        {
            deviceContext->ClearRenderTargetView(m_geometryBuffer.m_renderTargetViews[renderTargetIndex], backgroundColour);
        }
        deviceContext->ClearDepthStencilView(m_geometryBuffer.m_depthStencil.m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        // Update input assembler
        deviceContext->IASetInputLayout(ResourceManager::it().getDefaultVSPtr()->getInputLayout());
        deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // Set sampler state
        deviceContext->PSSetSamplers(0, 1, GraphicsHandler::it().getSamplerState().GetAddressOf());
        deviceContext->PSSetSamplers(1, 1, GraphicsHandler::it().getSamplerState().GetAddressOf());

        // Render and shade all objects
        if (GraphicsHandler::it().isUsingDeferredShading())
        {
            internalRenderDeferredShading(eyePos, eyeFacing, viewMatrix, projectionMatrix);
        }
        else
        {
            internalRenderStandard(eyePos, eyeFacing, viewMatrix, projectionMatrix);
        }

        deviceContext->RSSetState(GraphicsHandler::it().getDefaultRasterizerState());

        // Run post processes
        if (!m_postProcesses.empty())
        {
            for (int postProcessIndex = 0; postProcessIndex < m_postProcesses.size(); ++postProcessIndex)
            {
                if (postProcessIndex == 0)
                {
                    m_postProcesses[postProcessIndex]->run(&m_finalImage);
                }
                else
                {
                    m_postProcesses[postProcessIndex]->run(m_postProcesses[postProcessIndex - 1]->getResult());
                }
            }

            m_postProcesses.back()->getResult()->copyTo(&m_finalImage);
        }
    }

    RenderTexture& ImageRenderer::getFinalImage()
    {
        return m_finalImage;
    }

    GeometryBuffer* ImageRenderer::getGBuffer()
    {
        return &m_geometryBuffer;
    }

    void hrzn::gfx::ImageRenderer::release()
    {
        clearPostProcesses();

        m_geometryBuffer.release();

        m_finalImage.release();
        m_ambientOcclusion.release();
    }

    void ImageRenderer::internalRenderStandard(const DirectX::XMVECTOR& eyePos, const DirectX::XMVECTOR& eyeFacing, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix)
    {
        ID3D11DeviceContext* deviceContext = GraphicsHandler::it().getDeviceContext();

        // Set default blend state
        deviceContext->OMSetBlendState(GraphicsHandler::it().getDefaultBlendState(), NULL, 0xFFFFFFFF);

        // Render skybox
        GraphicsHandler::it().renderSkybox(eyePos);

        deviceContext->ClearDepthStencilView(m_geometryBuffer.m_depthStencil.m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        GraphicsHandler::it().renderSceneObjects(RenderPassType::eStandardPass, eyePos, eyeFacing);

        deviceContext->ClearDepthStencilView(GraphicsHandler::it().getDefaultDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        deviceContext->OMSetRenderTargets(4, m_geometryBuffer.m_renderTargetViews, GraphicsHandler::it().getDefaultDepthStencilView());

        GraphicsHandler::it().renderGizmos();

        // Unset gbuffer textures as render targets
        ID3D11RenderTargetView* const nullRenderTargetViews[8] = { NULL };
        deviceContext->OMSetRenderTargets(4, nullRenderTargetViews, nullptr);

        deviceContext->CopyResource(m_finalImage.m_texture2D.Get(), m_geometryBuffer.m_albedo.m_texture2D.Get());
    }

    void ImageRenderer::internalRenderDeferredShading(const DirectX::XMVECTOR& eyePos, const DirectX::XMVECTOR& eyeFacing, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix)
    {
        ID3D11DeviceContext* deviceContext = GraphicsHandler::it().getDeviceContext();

        // Set gbuffer blend state
        deviceContext->OMSetBlendState(GraphicsHandler::it().getGBufferBlendState(), NULL, 0xFFFFFFFF);

        // Render skybox to the gbuffer albedo texture
        GraphicsHandler::it().renderSkybox(eyePos);

        deviceContext->ClearDepthStencilView(m_geometryBuffer.m_depthStencil.m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        GraphicsHandler::it().renderSceneObjects(RenderPassType::eGBufferCompatiblePass, eyePos, eyeFacing);

        // Do SSAO stuff
        if (GraphicsHandler::it().isUsingSSAO())
        {
            // Unset render targets
            ID3D11RenderTargetView* const nullRenderTargetViews[8] = { NULL };
            deviceContext->OMSetRenderTargets(4, nullRenderTargetViews, nullptr);

            deviceContext->PSSetShaderResources(0, 1, m_geometryBuffer.m_depthStencil.m_shaderResourceView.GetAddressOf());
            deviceContext->PSSetShaderResources(1, 1, m_geometryBuffer.m_positionRoughness.m_shaderResourceView.GetAddressOf());
            deviceContext->PSSetShaderResources(2, 1, m_geometryBuffer.m_normalAO.m_shaderResourceView.GetAddressOf());
            deviceContext->PSSetShaderResources(3, 1, GraphicsHandler::it().getAmbientOcclusionNoiseShaderResourceView().GetAddressOf());

            deviceContext->PSSetConstantBuffers(1, 1, GraphicsHandler::it().getAmbientOcclusionCB().getAddressOf());

            deviceContext->PSSetShader(ResourceManager::it().getPSPtr("ambient_occlusion")->getShader(), NULL, 0);
            m_ambientOcclusion.setAsRenderTargetAndDrawQuad();

            deviceContext->PSSetShaderResources(4, 1, m_ambientOcclusion.m_shaderResourceView.GetAddressOf());
        }

        // Unset render targets and set as shader resource views
        ID3D11RenderTargetView* const nullRenderTargetViews[8] = { NULL };
        deviceContext->OMSetRenderTargets(4, nullRenderTargetViews, nullptr);
        deviceContext->PSSetShaderResources(0, 4, m_geometryBuffer.m_shaderResourceViews);

        // Shade the contents of the gbuffer into the final image texture using a quad
        deviceContext->PSSetShader(ResourceManager::it().getGBufferReadPSPtr()->getShader(), NULL, 0);
        m_finalImage.setAsRenderTargetAndDrawQuad();

        deviceContext->OMSetBlendState(GraphicsHandler::it().getDefaultBlendState(), NULL, 0xFFFFFFFF);

        deviceContext->IASetInputLayout(ResourceManager::it().getDefaultVSPtr()->getInputLayout());
        deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // Unset shader resource view and set as render targets once more to merge with nonCompatible pass
        ID3D11ShaderResourceView* const nullShaderResourceViews[8] = { NULL };
        deviceContext->PSSetShaderResources(0, 4, nullShaderResourceViews);
        deviceContext->OMSetRenderTargets(1, m_finalImage.m_renderTargetView.GetAddressOf(), m_geometryBuffer.m_depthStencil.m_depthStencilView.Get());

        GraphicsHandler::it().renderSceneObjects(RenderPassType::eNonGBufferCompatiblePass, eyePos, eyeFacing);

        deviceContext->OMSetRenderTargets(1, m_finalImage.m_renderTargetView.GetAddressOf(), GraphicsHandler::it().getDefaultDepthStencilView());
        deviceContext->ClearDepthStencilView(GraphicsHandler::it().getDefaultDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        GraphicsHandler::it().renderGizmos();

        deviceContext->OMSetRenderTargets(1, nullRenderTargetViews, nullptr);
    }
}