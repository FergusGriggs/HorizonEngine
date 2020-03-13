#pragma once

#include "../Model.h"
#include "../Texture.h"

#include <unordered_map>

enum class ResourceType {
	MODEL,
	TEXTURE
};

class ResourceManager
{
public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	Model* GetModelPtr(std::string path);
	Texture* GetTexturePtr(std::string path, aiTextureType textureType = aiTextureType::aiTextureType_DIFFUSE);
	Texture* GetTexturePtr(std::string path, const uint8_t* pData, size_t size, aiTextureType type);
	Texture* GetColourTexturePtr(std::string name, Colour colour, aiTextureType textureType = aiTextureType::aiTextureType_DIFFUSE);

private:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	int GetResourceIndex(ResourceType type, std::string path);

	std::vector<std::unordered_map<std::string, unsigned int>*> loadedResources;
	std::vector<Model*> models;
	std::vector<Texture*> textures;
};

