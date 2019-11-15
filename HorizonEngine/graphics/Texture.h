//Texture.h
//The Texture class is used to create ID3D11Resource and ID3D11ShaderResourceView objects from
//either a Colour object, or load in texture data using the available DirectX libraries

#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <assimp/material.h>

#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

#include "Colour.h"
#include "../utility/ErrorLogger.h"

enum class TextureStorageType
{
	INVALID,
	NONE,
	EMBEDDED_INDEX_COMPRESSED,
	EMBEDDED_INDEX_NON_COMPRESSED,
	EMBEDDED_COMPRESSED,
	EMBEDDED_NON_COMPRESSED,
	DISK
};

class Texture
{
public:
	Texture(ID3D11Device* device, const Colour& colour, aiTextureType type);
	Texture(ID3D11Device* device, const Colour* colourData, UINT width, UINT height, aiTextureType type);
	Texture(ID3D11Device* device, std::string & filePath, aiTextureType type);
	Texture(ID3D11Device* device, const uint8_t* pData, size_t size, aiTextureType type);

	aiTextureType GetType();
	ID3D11ShaderResourceView* GetTextureResourceView();
	ID3D11ShaderResourceView** GetTextureResourceViewAddress();

private:
	void Initialize1x1ColourTexture(ID3D11Device* device, const Colour& colour, aiTextureType type);
	void InitializeColourTexture(ID3D11Device* device, const Colour* colourData, UINT width, UINT height, aiTextureType type);
	Microsoft::WRL::ComPtr<ID3D11Resource> texture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView = nullptr;
	aiTextureType type = aiTextureType::aiTextureType_UNKNOWN;

};