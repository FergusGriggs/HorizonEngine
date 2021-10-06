#pragma once

#include <unordered_map>

#include "model.h"
#include "texture.h"

namespace hrzn::gfx
{
	enum class ResourceType
	{
		eModel,
		eTexture,
	};

	class ResourceManager
	{
	public:
		static ResourceManager& it();

		bool          initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
		gfx::Model*   getModelPtr(std::string path);
		gfx::Texture* getTexturePtr(std::string path, aiTextureType textureType = aiTextureType::aiTextureType_DIFFUSE);
		gfx::Texture* getTexturePtr(std::string path, const uint8_t* pData, size_t size, aiTextureType type);
		gfx::Texture* getColourTexturePtr(std::string name, Colour colour, aiTextureType textureType = aiTextureType::aiTextureType_DIFFUSE);

	private:
		int           getResourceIndex(ResourceType type, std::string path);

	private:
		std::vector<std::unordered_map<std::string, unsigned int>*> m_loadedResources;
		std::vector<gfx::Model*>   m_models;
		std::vector<gfx::Texture*> m_textures;

		ID3D11Device*        m_device;
		ID3D11DeviceContext* m_deviceContext;
	};
}
