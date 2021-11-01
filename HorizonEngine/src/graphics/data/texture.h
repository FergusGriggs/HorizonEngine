
//The Texture class is used to create ID3D11Resource and ID3D11ShaderResourceView objects from
//either a Colour object, or load in texture data using the available DirectX libraries

#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <assimp/material.h>

#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

#include "colour.h"
#include "../../utils/error_logger.h"

namespace hrzn::gfx
{
	enum class TextureStorageType
	{
		eInvalid,
		eNone,
		eEmbeddedIndexCompressed,
		eEmbeddedIndexNonCompressed,
		eEmbeddedCompressed,
		eEmbeddedNonCompressed,
		eDisk,
	};

	class Texture
	{
	public:
		Texture();
		Texture(ID3D11Device* device, const Colour& colour, aiTextureType type);
		Texture(ID3D11Device* device, const Colour* colourData, UINT width, UINT height, aiTextureType type);
		Texture(ID3D11Device* device, const std::string& filePath, aiTextureType type);
		Texture(ID3D11Device* device, const uint8_t* pData, size_t size, aiTextureType type);

		bool initialize(ID3D11Device* device, const std::string& filePath, aiTextureType type);

		aiTextureType              getType() const;
		ID3D11ShaderResourceView*  getTextureResourceView() const;
		ID3D11ShaderResourceView** getTextureResourceViewAddress();

	private:
		void initialize1x1ColourTexture(ID3D11Device* device, const Colour& colour, aiTextureType type);
		void initializeColourTexture(ID3D11Device* device, const Colour* colourData, UINT width, UINT height, aiTextureType type);

	private:
		aiTextureType                                    m_type;
		Microsoft::WRL::ComPtr<ID3D11Resource>           m_texture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureView;
	};
}