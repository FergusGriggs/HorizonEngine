
//Function implementations for the Texture class

#include "texture.h"

#include "../../utils/string_helpers.h"

namespace hrzn::gfx
{
	Texture::Texture() :
		m_type(aiTextureType::aiTextureType_UNKNOWN),
		m_texture(nullptr),
		m_textureView(nullptr)
	{
	}

	Texture::Texture(ID3D11Device* device, const Colour& colour, aiTextureType type) :
		Texture()
	{
		initialize1x1ColourTexture(device, colour, type);
	}

	Texture::Texture(ID3D11Device* device, const Colour* colourData, UINT width, UINT height, aiTextureType type) :
		Texture()
	{
		initializeColourTexture(device, colourData, width, height, type);
	}

	Texture::Texture(ID3D11Device* device, const std::string& filePath, aiTextureType type) :
		Texture()
	{
		m_type = type;
		if (utils::string_helpers::getFileExtension(filePath) == ".dds")
		{
			HRESULT hr = DirectX::CreateDDSTextureFromFile(device, utils::string_helpers::stringToWide(filePath).c_str(), m_texture.GetAddressOf(), m_textureView.GetAddressOf());
			if (FAILED(hr))
			{
				initialize1x1ColourTexture(device, colours::sc_unloadedTextureColour, m_type);
			}
			return;
		}
		else
		{
			HRESULT hr = DirectX::CreateWICTextureFromFile(device, utils::string_helpers::stringToWide(filePath).c_str(), m_texture.GetAddressOf(), m_textureView.GetAddressOf());
			if (FAILED(hr))
			{
				initialize1x1ColourTexture(device, colours::sc_unloadedTextureColour, m_type);
			}
			return;
		}
	}

	bool Texture::initialize(ID3D11Device* device, const std::string& filePath, aiTextureType type)
	{
		m_type = type;
		if (utils::string_helpers::getFileExtension(filePath) == ".dds")
		{
			HRESULT hr = DirectX::CreateDDSTextureFromFile(device, utils::string_helpers::stringToWide(filePath).c_str(), m_texture.GetAddressOf(), m_textureView.GetAddressOf());
			if (FAILED(hr))
			{
				initialize1x1ColourTexture(device, colours::sc_unloadedTextureColour, m_type);
			}
		}
		else
		{
			HRESULT hr = DirectX::CreateWICTextureFromFile(device, utils::string_helpers::stringToWide(filePath).c_str(), m_texture.GetAddressOf(), m_textureView.GetAddressOf());
			if (FAILED(hr))
			{
				initialize1x1ColourTexture(device, colours::sc_unloadedTextureColour, m_type);
			}
		}
		return true;
	}

	Texture::Texture(ID3D11Device* device, const uint8_t* pData, size_t size, aiTextureType type) :
		Texture()
	{
		m_type = type;
		HRESULT hr = DirectX::CreateWICTextureFromMemory(device, pData, size, m_texture.GetAddressOf(), m_textureView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create texture from memory.");
	}

	aiTextureType Texture::getType() const
	{
		return m_type;
	}

	ID3D11ShaderResourceView* Texture::getTextureResourceView() const
	{
		return m_textureView.Get();
	}

	ID3D11ShaderResourceView** Texture::getTextureResourceViewAddress()
	{
		return m_textureView.GetAddressOf();
	}

	void Texture::initialize1x1ColourTexture(ID3D11Device* device, const Colour& colour, aiTextureType type)
	{
		initializeColourTexture(device, &colour, 1, 1, type);
	}

	void Texture::initializeColourTexture(ID3D11Device* device, const Colour* colourData, UINT width, UINT height, aiTextureType type)
	{
		m_type = type;

		CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);

		ID3D11Texture2D* p2DTexture = nullptr;

		D3D11_SUBRESOURCE_DATA initialData{};
		initialData.pSysMem = colourData;
		initialData.SysMemPitch = width * sizeof(Colour);

		HRESULT hr = device->CreateTexture2D(&textureDesc, &initialData, &p2DTexture);

		COM_ERROR_IF_FAILED(hr, "Failed to initialize texture from colour data.");

		m_texture = p2DTexture;

		CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);
		hr = device->CreateShaderResourceView(m_texture.Get(), &shaderResourceViewDesc, m_textureView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create shader resource view from texture from generated.");
	}
}