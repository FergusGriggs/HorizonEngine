#include "geometry_buffer.h"

#include "../../user_config.h"
#include "../../utils/error_logger.h"

namespace hrzn::gfx
{
	GeometryBuffer::GeometryBuffer()
	{
	}

	GeometryBuffer::~GeometryBuffer()
	{
	}

	void GeometryBuffer::initialise(ID3D11Device* device)
	{
		// Create texture descriptions
		D3D11_TEXTURE2D_DESC unsignedNormalizedTextureDesc{};
		unsignedNormalizedTextureDesc.Width = UserConfig::it().getWindowWidthFloat();
		unsignedNormalizedTextureDesc.Height = UserConfig::it().getWindowHeightFloat();
		unsignedNormalizedTextureDesc.MipLevels = 1;
		unsignedNormalizedTextureDesc.ArraySize = 1;
		unsignedNormalizedTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		unsignedNormalizedTextureDesc.SampleDesc.Count = 1;
		unsignedNormalizedTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		unsignedNormalizedTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		D3D11_TEXTURE2D_DESC signedFloatTextureDesc{};
		signedFloatTextureDesc.Width = UserConfig::it().getWindowWidthFloat();
		signedFloatTextureDesc.Height = UserConfig::it().getWindowHeightFloat();
		signedFloatTextureDesc.MipLevels = 1;
		signedFloatTextureDesc.ArraySize = 1;
		signedFloatTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		signedFloatTextureDesc.SampleDesc.Count = 1;
		signedFloatTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		signedFloatTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		D3D11_TEXTURE2D_DESC depthStencilTextureDesc;
		depthStencilTextureDesc.Width = UserConfig::it().getWindowWidthFloat();
		depthStencilTextureDesc.Height = UserConfig::it().getWindowHeightFloat();
		depthStencilTextureDesc.MipLevels = 1;
		depthStencilTextureDesc.ArraySize = 1;
		depthStencilTextureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		depthStencilTextureDesc.SampleDesc.Count = 1;
		depthStencilTextureDesc.SampleDesc.Quality = 0;
		depthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		depthStencilTextureDesc.CPUAccessFlags = 0;
		depthStencilTextureDesc.MiscFlags = 0;

		// Create textures
		// Albedo
		HRESULT hr = device->CreateTexture2D(&unsignedNormalizedTextureDesc, nullptr, m_albedo.m_texture2D.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer albedo texture creation failed");

		// PositionRoughness
		hr = device->CreateTexture2D(&signedFloatTextureDesc, nullptr, m_positionRoughness.m_texture2D.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer position roughness texture creation failed");

		// NormalAO
		hr = device->CreateTexture2D(&signedFloatTextureDesc, nullptr, m_normalAO.m_texture2D.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer normal ao texture creation failed");

		// EmissionMetallic
		hr = device->CreateTexture2D(&unsignedNormalizedTextureDesc, nullptr, m_emissionMetallic.m_texture2D.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer emission metallic texture creation failed");

		// DepthStencil
		hr = device->CreateTexture2D(&depthStencilTextureDesc, nullptr, m_depthStencil.m_texture2D.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer depth stencil texture creation failed");


		// Create render target view descriptions
		D3D11_RENDER_TARGET_VIEW_DESC unsignedNormalizedRenderTargetViewDesc{};
		unsignedNormalizedRenderTargetViewDesc.Format = unsignedNormalizedTextureDesc.Format;
		unsignedNormalizedRenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		D3D11_RENDER_TARGET_VIEW_DESC signedFloatRenderTargetViewDesc{};
		signedFloatRenderTargetViewDesc.Format = signedFloatTextureDesc.Format;
		signedFloatRenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		// Create render target views
		// Albedo
		hr = device->CreateRenderTargetView(m_albedo.m_texture2D.Get(), &unsignedNormalizedRenderTargetViewDesc, m_albedo.m_renderTargetView.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer albedo render target view creation failed");

		// PositionRoughness
		hr = device->CreateRenderTargetView(m_positionRoughness.m_texture2D.Get(), &signedFloatRenderTargetViewDesc, m_positionRoughness.m_renderTargetView.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer position roughness render target view creation failed");

		// NormalAO
		hr = device->CreateRenderTargetView(m_normalAO.m_texture2D.Get(), &signedFloatRenderTargetViewDesc, m_normalAO.m_renderTargetView.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer normal ao render target view creation failed");

		// EmissionMetallic
		hr = device->CreateRenderTargetView(m_emissionMetallic.m_texture2D.Get(), &unsignedNormalizedRenderTargetViewDesc, m_emissionMetallic.m_renderTargetView.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer emission metallic render target view creation failed");


		// Create depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		depthStencilViewDesc.Flags = 0;
		depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		// DepthStencil
		hr = device->CreateDepthStencilView(m_depthStencil.m_texture2D.Get(), &depthStencilViewDesc, m_depthStencil.m_depthStencilView.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer depth stencil view creation failed");


		// Create shader resource view descriptions
		D3D11_SHADER_RESOURCE_VIEW_DESC unsignedNormalizedShaderResourceViewDesc{};
		unsignedNormalizedShaderResourceViewDesc.Format = unsignedNormalizedTextureDesc.Format;
		unsignedNormalizedShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		unsignedNormalizedShaderResourceViewDesc.Texture2D.MipLevels = 1;

		D3D11_SHADER_RESOURCE_VIEW_DESC signedFloatShaderResourceViewDesc{};
		signedFloatShaderResourceViewDesc.Format = signedFloatTextureDesc.Format;
		signedFloatShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		signedFloatShaderResourceViewDesc.Texture2D.MipLevels = 1;

		D3D11_SHADER_RESOURCE_VIEW_DESC depthStencilShaderResourceViewDesc;
		depthStencilShaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
		depthStencilShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		depthStencilShaderResourceViewDesc.Texture2D.MipLevels = depthStencilTextureDesc.MipLevels;
		depthStencilShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

		// Create shader resource views
		// Albedo
		hr = device->CreateShaderResourceView(m_albedo.m_texture2D.Get(), &unsignedNormalizedShaderResourceViewDesc, m_albedo.m_shaderResourceView.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer albedo shader resource view creation failed");

		// PositionRoughness
		hr = device->CreateShaderResourceView(m_positionRoughness.m_texture2D.Get(), &signedFloatShaderResourceViewDesc, m_positionRoughness.m_shaderResourceView.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer position roughness shader resource view creation failed");

		// NormalAO
		hr = device->CreateShaderResourceView(m_normalAO.m_texture2D.Get(), &signedFloatShaderResourceViewDesc, m_normalAO.m_shaderResourceView.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer normal ao shader resource view creation failed");

		// EmissionMetallic
		hr = device->CreateShaderResourceView(m_emissionMetallic.m_texture2D.Get(), &unsignedNormalizedShaderResourceViewDesc, m_emissionMetallic.m_shaderResourceView.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer emission metallic shader resource view creation failed");

		// Depth Stencil
		hr = device->CreateShaderResourceView(m_depthStencil.m_texture2D.Get(), &depthStencilShaderResourceViewDesc, m_depthStencil.m_shaderResourceView.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer depth stencil shader resource view creation failed");


		// Fill geometry buffer render target array
		m_renderTargetViews[0] = m_albedo.m_renderTargetView.Get();
		m_renderTargetViews[1] = m_positionRoughness.m_renderTargetView.Get();
		m_renderTargetViews[2] = m_normalAO.m_renderTargetView.Get();
		m_renderTargetViews[3] = m_emissionMetallic.m_renderTargetView.Get();

		// Fill geometry buffer shader resource view array
		m_shaderResourceViews[0] = m_albedo.m_shaderResourceView.Get();
		m_shaderResourceViews[1] = m_positionRoughness.m_shaderResourceView.Get();
		m_shaderResourceViews[2] = m_normalAO.m_shaderResourceView.Get();
		m_shaderResourceViews[3] = m_emissionMetallic.m_shaderResourceView.Get();
	}
}
