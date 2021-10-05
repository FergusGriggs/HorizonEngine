#include "ResourceManager.h"


bool ResourceManager::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	this->device = device;
	this->deviceContext = deviceContext;

	if (this->device == nullptr || this->deviceContext == nullptr)
	{
		return false;
	}

	for (int i = 0; i < 2; i++)
	{
		std::unordered_map<std::string, unsigned int>* resourceMap = new std::unordered_map<std::string, unsigned int>;
		this->loadedResources.push_back(resourceMap);
	}
	
	return true;
}

Model* ResourceManager::GetModelPtr(std::string path)
{
	int modelIndex = GetResourceIndex(ResourceType::MODEL, path);
	if (modelIndex >= 0)
	{
		return models.at(modelIndex);
	}
	else
	{
		Model* loadedModel = new Model();
		if (loadedModel->Initialize(path, this->device, this->deviceContext, this))
		{
			models.push_back(loadedModel);
			loadedResources.at(static_cast<int>(ResourceType::MODEL))->insert(std::make_pair(path, models.size() - 1));
			return loadedModel;
		}
		else
		{
			delete loadedModel;
			models.push_back(nullptr);
			loadedResources.at(static_cast<int>(ResourceType::MODEL))->insert(std::make_pair(path, models.size() - 1));
			return nullptr;
		}
	}
}

Texture* ResourceManager::GetTexturePtr(std::string path, aiTextureType textureType)
{
	int textureIndex = GetResourceIndex(ResourceType::TEXTURE, path);
	if (textureIndex >= 0)
	{
		return textures.at(textureIndex);
	}
	else
	{
		Texture* loadedTexture = new Texture();
		if (loadedTexture->Initialize(this->device, path, textureType))
		{
			textures.push_back(loadedTexture);
			loadedResources.at(static_cast<int>(ResourceType::TEXTURE))->insert(std::make_pair(path, textures.size() - 1));
			return loadedTexture;
		}
		else
		{
			delete loadedTexture;
			models.push_back(nullptr);
			loadedResources.at(static_cast<int>(ResourceType::TEXTURE))->insert(std::make_pair(path, textures.size() - 1));
			return nullptr;
		}
	}
}

Texture* ResourceManager::GetTexturePtr(std::string path, const uint8_t* pData, size_t size, aiTextureType type)
{
	int textureIndex = GetResourceIndex(ResourceType::TEXTURE, path);
	if (textureIndex >= 0)
	{
		return textures.at(textureIndex);
	}
	else
	{
		Texture* newTexture = new Texture(this->device, pData, size, type);
		textures.push_back(newTexture);
		loadedResources.at(static_cast<int>(ResourceType::TEXTURE))->insert(std::make_pair(path, textures.size() - 1));
		return newTexture;
	}
}

Texture* ResourceManager::GetColourTexturePtr(std::string name, Colour colour, aiTextureType textureType)
{
	int textureIndex = GetResourceIndex(ResourceType::TEXTURE, name);
	if (textureIndex >= 0)
	{
		return textures.at(textureIndex);
	}
	else
	{
		Texture* newTexture = new Texture(this->device, colour, textureType);
		textures.push_back(newTexture);
		loadedResources.at(static_cast<int>(ResourceType::TEXTURE))->insert(std::make_pair(name, textures.size() - 1));
		return newTexture;
	}
}

int ResourceManager::GetResourceIndex(ResourceType type, std::string path)
{
	auto resourceMap = loadedResources.at(static_cast<int>(type));
	if (resourceMap->find(path) != resourceMap->end())
	{
		return resourceMap->at(path);
	}
	else
	{
		return -1;
	}
}
