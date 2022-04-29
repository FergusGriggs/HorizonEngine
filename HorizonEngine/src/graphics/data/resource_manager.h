#pragma once

#include <unordered_map>

#include "texture.h"
#include "material.h"
#include "../shaders/vertex_shader.h"
#include "../shaders/pixel_shader.h"
#include "../shaders/compute_shader.h"

namespace hrzn::gfx
{
	class Model;
}

namespace hrzn::gfx
{
	class ResourceManager
	{
	public:
		static ResourceManager& it();

		bool initialise();

		template<class VertexType>
		Model*    getModelPtr(const std::string& path);

		Texture*  getTexturePtr(const std::string& path);
		Texture*  getTexturePtr(const std::string& path, const uint8_t* pData, size_t size);
		Texture*  getColourTexturePtr(Colour colour);

		Material* getMaterialPtr(const std::string& name);

		VertexShader*  getVSPtr(const std::string& name);
		PixelShader*   getPSPtr(const std::string& name);
		PixelShader*   getGBufferWritePSPtr(const std::string& name);
		ComputeShader* getCSPtr(const std::string& name);

		Model*         getDefaultModelPtr();
		Texture*       getDefaultTexturePtr();
		Material*      getDefaultMaterialPtr();
		VertexShader*  getDefaultVSPtr();
		PixelShader*   getDefaultPSPtr();
		PixelShader*   getDefaultGBufferWritePSPtr();
		ComputeShader* getDefaultCSPtr();
		PixelShader*   getGBufferReadPSPtr();

	private:
		ResourceManager();

	private:
		std::unordered_map<std::string, Model*>       m_models;

		std::unordered_map<std::string, Texture*>     m_textures;
		std::unordered_map<unsigned int, Texture*>    m_colourTextures;

		std::unordered_map<std::string, Material*>    m_materials;

		std::unordered_map<std::string, VertexShader*>  m_vertexShaders;
		std::unordered_map<std::string, PixelShader*>   m_pixelShaders;
		std::unordered_map<std::string, PixelShader*>   m_gBufferWritePixelShaders;
		std::unordered_map<std::string, ComputeShader*> m_computeShaders;

		Model*         m_defaultModel;
		Texture*       m_defaultTexture;
		Material*      m_defaultMaterial;

		VertexShader*  m_defaultVS;
		PixelShader*   m_defaultPS;
		PixelShader*   m_defaultGBufferWritePS;
		ComputeShader* m_defaultCS;

		PixelShader*   m_gBufferReadPS;
	};
}

#include "model.h"

namespace hrzn::gfx
{
	template<class VertexType>
	inline Model* ResourceManager::getModelPtr(const std::string& path)
	{
		const auto& modelItr = m_models.find(path);
		if (modelItr != m_models.end())
		{
			return modelItr->second;
		}
		else
		{
			Model* loadedModel = new Model();
			if (!loadedModel->initialise<VertexType>(path))
			{
				delete loadedModel;
				loadedModel = m_defaultModel;
			}
			m_models.insert({ path, loadedModel });
			return loadedModel;
		}
	}
}
