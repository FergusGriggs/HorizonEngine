#include "resource_manager.h"

namespace hrzn::gfx
{
	ResourceManager& ResourceManager::it()
	{
		static ResourceManager instance;
		return instance;
	}

	bool ResourceManager::initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		m_device = device;
		m_deviceContext = deviceContext;

		if (m_device == nullptr || m_deviceContext == nullptr)
		{
			return false;
		}

		for (int i = 0; i < 2; i++)
		{
			std::unordered_map<std::string, unsigned int>* resourceMap = new std::unordered_map<std::string, unsigned int>;
			m_loadedResources.push_back(resourceMap);
		}

		return true;
	}

	Model* ResourceManager::getModelPtr(std::string path)
	{
		int modelIndex = getResourceIndex(ResourceType::eModel, path);
		if (modelIndex >= 0)
		{
			return m_models.at(modelIndex);
		}
		else
		{
			Model* loadedModel = new Model();
			if (loadedModel->initialize(path, m_device, m_deviceContext))
			{
				m_models.push_back(loadedModel);
				m_loadedResources.at(static_cast<int>(ResourceType::eModel))->insert(std::make_pair(path, m_models.size() - 1));
				return loadedModel;
			}
			else
			{
				delete loadedModel;
				m_models.push_back(nullptr);
				m_loadedResources.at(static_cast<int>(ResourceType::eModel))->insert(std::make_pair(path, m_models.size() - 1));
				return nullptr;
			}
		}
	}

	Texture* ResourceManager::getTexturePtr(std::string path, aiTextureType textureType)
	{
		int textureIndex = getResourceIndex(ResourceType::eTexture, path);
		if (textureIndex >= 0)
		{
			return m_textures.at(textureIndex);
		}
		else
		{
			Texture* loadedTexture = new Texture();
			if (loadedTexture->initialize(m_device, path, textureType))
			{
				m_textures.push_back(loadedTexture);
				m_loadedResources.at(static_cast<int>(ResourceType::eTexture))->insert(std::make_pair(path, m_textures.size() - 1));
				return loadedTexture;
			}
			else
			{
				delete loadedTexture;
				m_models.push_back(nullptr);
				m_loadedResources.at(static_cast<int>(ResourceType::eTexture))->insert(std::make_pair(path, m_textures.size() - 1));
				return nullptr;
			}
		}
	}

	Texture* ResourceManager::getTexturePtr(std::string path, const uint8_t* pData, size_t size, aiTextureType type)
	{
		int textureIndex = getResourceIndex(ResourceType::eTexture, path);
		if (textureIndex >= 0)
		{
			return m_textures.at(textureIndex);
		}
		else
		{
			Texture* newTexture = new Texture(m_device, pData, size, type);
			m_textures.push_back(newTexture);
			m_loadedResources.at(static_cast<int>(ResourceType::eTexture))->insert(std::make_pair(path, m_textures.size() - 1));
			return newTexture;
		}
	}

	Texture* ResourceManager::getColourTexturePtr(std::string name, Colour colour, aiTextureType textureType)
	{
		int textureIndex = getResourceIndex(ResourceType::eTexture, name);
		if (textureIndex >= 0)
		{
			return m_textures.at(textureIndex);
		}
		else
		{
			Texture* newTexture = new Texture(m_device, colour, textureType);
			m_textures.push_back(newTexture);
			m_loadedResources.at(static_cast<int>(ResourceType::eTexture))->insert(std::make_pair(name, m_textures.size() - 1));
			return newTexture;
		}
	}

	int ResourceManager::getResourceIndex(ResourceType type, std::string path)
	{
		auto resourceMap = m_loadedResources.at(static_cast<int>(type));
		if (resourceMap->find(path) != resourceMap->end())
		{
			return resourceMap->at(path);
		}
		else
		{
			return -1;
		}
	}
}
