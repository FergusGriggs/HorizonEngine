
//The Texture class is used to create ID3D11Resource and ID3D11ShaderResourceView objects from
//either a Colour object, or load in texture data using the available DirectX libraries

#pragma once

#include <d3d11.h>
#include <wrl/client.h>

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
		Texture(const Colour& colour);
		Texture(const Colour* colourData, UINT width, UINT height);
		Texture(const std::string& filePath);
		Texture(const uint8_t* pData, size_t size);

		bool initialise(const std::string& filePath);

		Microsoft::WRL::ComPtr<ID3D11Resource>&           getResource();
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& getShaderResourceView();

	private:
		void initialize1x1ColourTexture(const Colour& colour);
		void initializeColourTexture(const Colour* colourData, UINT width, UINT height);

	private:
		Microsoft::WRL::ComPtr<ID3D11Resource>           m_texture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureView;
	};
}