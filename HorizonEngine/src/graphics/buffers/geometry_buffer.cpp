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
		// Albedo
		// Emission Metallic
		D3D11_TEXTURE2D_DESC R8G8B8A8_UNORM_TextureDesc{};
		R8G8B8A8_UNORM_TextureDesc.Width = UserConfig::it().getWindowWidthFloat();
		R8G8B8A8_UNORM_TextureDesc.Height = UserConfig::it().getWindowHeightFloat();
		R8G8B8A8_UNORM_TextureDesc.MipLevels = 1;
		R8G8B8A8_UNORM_TextureDesc.ArraySize = 1;
		R8G8B8A8_UNORM_TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		R8G8B8A8_UNORM_TextureDesc.SampleDesc.Count = 1;
		R8G8B8A8_UNORM_TextureDesc.Usage = D3D11_USAGE_DEFAULT;
		R8G8B8A8_UNORM_TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		// Normal AO
		D3D11_TEXTURE2D_DESC R16G16B16A16_UNORM_TextureDesc{};
		R16G16B16A16_UNORM_TextureDesc.Width = UserConfig::it().getWindowWidthFloat();
		R16G16B16A16_UNORM_TextureDesc.Height = UserConfig::it().getWindowHeightFloat();
		R16G16B16A16_UNORM_TextureDesc.MipLevels = 1;
		R16G16B16A16_UNORM_TextureDesc.ArraySize = 1;
		R16G16B16A16_UNORM_TextureDesc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
		R16G16B16A16_UNORM_TextureDesc.SampleDesc.Count = 1;
		R16G16B16A16_UNORM_TextureDesc.Usage = D3D11_USAGE_DEFAULT;
		R16G16B16A16_UNORM_TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		// Height
		D3D11_TEXTURE2D_DESC R16_UNORM_TextureDesc{};
		R16_UNORM_TextureDesc.Width = UserConfig::it().getWindowWidthFloat();
		R16_UNORM_TextureDesc.Height = UserConfig::it().getWindowHeightFloat();
		R16_UNORM_TextureDesc.MipLevels = 1;
		R16_UNORM_TextureDesc.ArraySize = 1;
		R16_UNORM_TextureDesc.Format = DXGI_FORMAT_R16_UNORM;
		R16_UNORM_TextureDesc.SampleDesc.Count = 1;
		R16_UNORM_TextureDesc.Usage = D3D11_USAGE_DEFAULT;
		R16_UNORM_TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		// Position
		D3D11_TEXTURE2D_DESC R32G32B32A32_FLOAT_TextureDesc{};
		R32G32B32A32_FLOAT_TextureDesc.Width = UserConfig::it().getWindowWidthFloat();
		R32G32B32A32_FLOAT_TextureDesc.Height = UserConfig::it().getWindowHeightFloat();
		R32G32B32A32_FLOAT_TextureDesc.MipLevels = 1;
		R32G32B32A32_FLOAT_TextureDesc.ArraySize = 1;
		R32G32B32A32_FLOAT_TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		R32G32B32A32_FLOAT_TextureDesc.SampleDesc.Count = 1;
		R32G32B32A32_FLOAT_TextureDesc.Usage = D3D11_USAGE_DEFAULT;
		R32G32B32A32_FLOAT_TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		// Depth Stencil
		D3D11_TEXTURE2D_DESC depthStencilTextureDesc;
		depthStencilTextureDesc.Width = UserConfig::it().getWindowWidth();
		depthStencilTextureDesc.Height = UserConfig::it().getWindowHeight();
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
		HRESULT hr = device->CreateTexture2D(&R8G8B8A8_UNORM_TextureDesc, nullptr, m_albedo.m_texture2D.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer albedo texture creation failed");

		// PositionRoughness
		hr = device->CreateTexture2D(&R32G32B32A32_FLOAT_TextureDesc, nullptr, m_positionRoughness.m_texture2D.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer position roughness texture creation failed");

		// NormalAO
		hr = device->CreateTexture2D(&R16G16B16A16_UNORM_TextureDesc, nullptr, m_normalAO.m_texture2D.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer normal ao texture creation failed");

		// Height
		hr = device->CreateTexture2D(&R16_UNORM_TextureDesc, nullptr, m_height.m_texture2D.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer height texture creation failed");

		// EmissionMetallic
		hr = device->CreateTexture2D(&R8G8B8A8_UNORM_TextureDesc, nullptr, m_emissionMetallic.m_texture2D.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer emission metallic texture creation failed");

		// DepthStencil
		hr = device->CreateTexture2D(&depthStencilTextureDesc, nullptr, m_depthStencil.m_texture2D.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer depth stencil texture creation failed");


		// Create render target view descriptions
		D3D11_RENDER_TARGET_VIEW_DESC R8G8B8A8_UNORM_RenderTargetViewDesc{};
		R8G8B8A8_UNORM_RenderTargetViewDesc.Format = R8G8B8A8_UNORM_TextureDesc.Format;
		R8G8B8A8_UNORM_RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		D3D11_RENDER_TARGET_VIEW_DESC R16G16B16A16_UNORM_RenderTargetViewDesc{};
		R16G16B16A16_UNORM_RenderTargetViewDesc.Format = R16G16B16A16_UNORM_TextureDesc.Format;
		R16G16B16A16_UNORM_RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		D3D11_RENDER_TARGET_VIEW_DESC R16_UNORM_RenderTargetViewDesc{};
		R16_UNORM_RenderTargetViewDesc.Format = R16_UNORM_TextureDesc.Format;
		R16_UNORM_RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		D3D11_RENDER_TARGET_VIEW_DESC R32G32B32A32_FLOAT_RenderTargetViewDesc{};
		R32G32B32A32_FLOAT_RenderTargetViewDesc.Format = R32G32B32A32_FLOAT_TextureDesc.Format;
		R32G32B32A32_FLOAT_RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		// Create render target views
		// Albedo
		hr = device->CreateRenderTargetView(m_albedo.m_texture2D.Get(), &R8G8B8A8_UNORM_RenderTargetViewDesc, m_albedo.m_renderTargetView.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer albedo render target view creation failed");

		// PositionRoughness
		hr = device->CreateRenderTargetView(m_positionRoughness.m_texture2D.Get(), &R32G32B32A32_FLOAT_RenderTargetViewDesc, m_positionRoughness.m_renderTargetView.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer position roughness render target view creation failed");

		// NormalAO
		hr = device->CreateRenderTargetView(m_normalAO.m_texture2D.Get(), &R16G16B16A16_UNORM_RenderTargetViewDesc, m_normalAO.m_renderTargetView.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer normal ao render target view creation failed");

		// Height
		hr = device->CreateRenderTargetView(m_height.m_texture2D.Get(), &R16_UNORM_RenderTargetViewDesc, m_height.m_renderTargetView.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer height render target view creation failed");

		// EmissionMetallic
		hr = device->CreateRenderTargetView(m_emissionMetallic.m_texture2D.Get(), &R8G8B8A8_UNORM_RenderTargetViewDesc, m_emissionMetallic.m_renderTargetView.GetAddressOf());
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
		D3D11_SHADER_RESOURCE_VIEW_DESC R8G8B8A8_UNORM_ShaderResourceViewDesc{};
		R8G8B8A8_UNORM_ShaderResourceViewDesc.Format = R8G8B8A8_UNORM_TextureDesc.Format;
		R8G8B8A8_UNORM_ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		R8G8B8A8_UNORM_ShaderResourceViewDesc.Texture2D.MipLevels = 1;

		D3D11_SHADER_RESOURCE_VIEW_DESC R16G16B16A16_UNORM_ShaderResourceViewDesc{};
		R16G16B16A16_UNORM_ShaderResourceViewDesc.Format = R16G16B16A16_UNORM_TextureDesc.Format;
		R16G16B16A16_UNORM_ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		R16G16B16A16_UNORM_ShaderResourceViewDesc.Texture2D.MipLevels = 1;

		D3D11_SHADER_RESOURCE_VIEW_DESC R16_UNORM_ShaderResourceViewDesc{};
		R16_UNORM_ShaderResourceViewDesc.Format = R16_UNORM_TextureDesc.Format;
		R16_UNORM_ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		R16_UNORM_ShaderResourceViewDesc.Texture2D.MipLevels = 1;

		D3D11_SHADER_RESOURCE_VIEW_DESC R32G32B32A32_FLOAT_ShaderResourceViewDesc{};
		R32G32B32A32_FLOAT_ShaderResourceViewDesc.Format = R32G32B32A32_FLOAT_TextureDesc.Format;
		R32G32B32A32_FLOAT_ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		R32G32B32A32_FLOAT_ShaderResourceViewDesc.Texture2D.MipLevels = 1;

		D3D11_SHADER_RESOURCE_VIEW_DESC depthStencilShaderResourceViewDesc;
		depthStencilShaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
		depthStencilShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		depthStencilShaderResourceViewDesc.Texture2D.MipLevels = depthStencilTextureDesc.MipLevels;
		depthStencilShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

		// Create shader resource views
		// Albedo
		hr = device->CreateShaderResourceView(m_albedo.m_texture2D.Get(), &R8G8B8A8_UNORM_ShaderResourceViewDesc, m_albedo.m_shaderResourceView.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer albedo shader resource view creation failed");

		// PositionRoughness
		hr = device->CreateShaderResourceView(m_positionRoughness.m_texture2D.Get(), &R32G32B32A32_FLOAT_ShaderResourceViewDesc, m_positionRoughness.m_shaderResourceView.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer position roughness shader resource view creation failed");

		// NormalAO
		hr = device->CreateShaderResourceView(m_normalAO.m_texture2D.Get(), &R16G16B16A16_UNORM_ShaderResourceViewDesc, m_normalAO.m_shaderResourceView.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer normal ao shader resource view creation failed");

		// Height
		hr = device->CreateShaderResourceView(m_height.m_texture2D.Get(), &R16_UNORM_ShaderResourceViewDesc, m_height.m_shaderResourceView.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer height shader resource view creation failed");

		// EmissionMetallic
		hr = device->CreateShaderResourceView(m_emissionMetallic.m_texture2D.Get(), &R8G8B8A8_UNORM_ShaderResourceViewDesc, m_emissionMetallic.m_shaderResourceView.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer emission metallic shader resource view creation failed");

		// Depth Stencil
		hr = device->CreateShaderResourceView(m_depthStencil.m_texture2D.Get(), &depthStencilShaderResourceViewDesc, m_depthStencil.m_shaderResourceView.GetAddressOf());
		utils::ErrorLogger::logIfFailed(hr, "Geometry buffer depth stencil shader resource view creation failed");


		// Fill geometry buffer render target array
		m_renderTargetViews[0] = m_albedo.m_renderTargetView.Get();
		m_renderTargetViews[1] = m_positionRoughness.m_renderTargetView.Get();
		m_renderTargetViews[2] = m_normalAO.m_renderTargetView.Get();
		m_renderTargetViews[3] = m_height.m_renderTargetView.Get();
		m_renderTargetViews[4] = m_emissionMetallic.m_renderTargetView.Get();

		// Fill geometry buffer shader resource view array
		m_shaderResourceViews[0] = m_albedo.m_shaderResourceView.Get();
		m_shaderResourceViews[1] = m_positionRoughness.m_shaderResourceView.Get();
		m_shaderResourceViews[2] = m_normalAO.m_shaderResourceView.Get();
		m_shaderResourceViews[3] = m_height.m_shaderResourceView.Get();
		m_shaderResourceViews[4] = m_emissionMetallic.m_shaderResourceView.Get();
	}
}
