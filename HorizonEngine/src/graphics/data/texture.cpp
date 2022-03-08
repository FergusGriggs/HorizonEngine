
//Function implementations for the Texture class

#include "texture.h"

#include "../graphics_handler.h"

#include "../../utils/string_helpers.h"

namespace hrzn::gfx
{
	Texture::Texture() :
		m_texture(nullptr),
		m_textureView(nullptr)
	{
	}

	Texture::Texture(const Colour& colour) :
		Texture()
	{
		initialize1x1ColourTexture(colour);
	}

	Texture::Texture(const Colour* colourData, UINT width, UINT height) :
		Texture()
	{
		initializeColourTexture(colourData, width, height);
	}

	Texture::Texture(const std::string& filePath) :
		Texture()
	{
		initialize(filePath);
	}

	bool Texture::initialize(const std::string& filePath)
	{
		if (utils::string_helpers::getFileExtension(filePath) == "dds")
		{
			HRESULT hr = DirectX::CreateDDSTextureFromFile(GraphicsHandler::it().getDevice(), utils::string_helpers::stringToWide(filePath).c_str(), m_texture.GetAddressOf(), m_textureView.GetAddressOf());
			if (FAILED(hr))
			{
				initialize1x1ColourTexture(colours::sc_unhandledTextureColour);
			}
		}
		else
		{
			HRESULT hr = DirectX::CreateWICTextureFromFile(GraphicsHandler::it().getDevice(), utils::string_helpers::stringToWide(filePath).c_str(), m_texture.GetAddressOf(), m_textureView.GetAddressOf());
			if (FAILED(hr))
			{
				initialize1x1ColourTexture(colours::sc_unhandledTextureColour);
			}
		}
		return true;
	}

	Texture::Texture(const uint8_t* pData, size_t size) :
		Texture()
	{
		HRESULT hr = DirectX::CreateWICTextureFromMemory(GraphicsHandler::it().getDevice(), pData, size, m_texture.GetAddressOf(), m_textureView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create texture from memory.");
	}

	ID3D11ShaderResourceView* Texture::getTextureResourceView() const
	{
		return m_textureView.Get();
	}

	ID3D11ShaderResourceView** Texture::getTextureResourceViewAddress()
	{
		return m_textureView.GetAddressOf();
	}

	void Texture::initialize1x1ColourTexture(const Colour& colour)
	{
		initializeColourTexture(&colour, 1, 1);
	}

	void Texture::initializeColourTexture(const Colour* colourData, UINT width, UINT height)
	{
		CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);

		ID3D11Texture2D* p2DTexture = nullptr;

		D3D11_SUBRESOURCE_DATA initialData{};
		initialData.pSysMem = colourData;
		initialData.SysMemPitch = width * sizeof(Colour);

		HRESULT hr = GraphicsHandler::it().getDevice()->CreateTexture2D(&textureDesc, &initialData, &p2DTexture);

		COM_ERROR_IF_FAILED(hr, "Failed to initialize texture from colour data.");

		m_texture = p2DTexture;

		CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);
		hr = GraphicsHandler::it().getDevice()->CreateShaderResourceView(m_texture.Get(), &shaderResourceViewDesc, m_textureView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create shader resource view from texture from generated.");
	}
}