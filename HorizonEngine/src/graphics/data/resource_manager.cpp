#include "resource_manager.h"

#include "../graphics_handler.h"

#include "../../utils/string_helpers.h"

namespace hrzn::gfx
{
	ResourceManager& ResourceManager::it()
	{
		static ResourceManager instance;
		return instance;
	}

	bool ResourceManager::initialise()
	{
		m_defaultTexture = getTexturePtr("res/textures/missing.png");
		m_defaultMaterial = getMaterialPtr("default");
		m_defaultModel = getModelPtr("res/models/engine/test/horizon_statue.obj");

		m_defaultVS = getVSPtr("default");
		m_defaultPS = getPSPtr("default");
		m_defaultGBufferWritePS = getGBufferWritePSPtr("default");
		m_defaultCS = nullptr;

		m_gBufferReadPS = getPSPtr("gbuf_r_default");

		return true;
	}

	Model* ResourceManager::getModelPtr(std::string path)
	{
		const auto& modelItr = m_models.find(path);
		if (modelItr != m_models.end())
		{
			return modelItr->second;
		}
		else
		{
			Model* loadedModel = new Model();
			if (!loadedModel->initialize(path))
			{
				delete loadedModel;
				loadedModel = m_defaultModel;
			}
			m_models.insert({ path, loadedModel });
			return loadedModel;
		}
	}

	Texture* ResourceManager::getTexturePtr(std::string path)
	{
		const auto& textureItr = m_textures.find(path);
		if (textureItr != m_textures.end())
		{
			return textureItr->second;
		}
		else
		{
			Texture* loadedTexture = new Texture();
			if (!loadedTexture->initialize(path))
			{
				delete loadedTexture;
				loadedTexture = m_defaultTexture;
			}
			m_textures.insert({ path, loadedTexture });
			return loadedTexture;
		}
	}

	Texture* ResourceManager::getTexturePtr(std::string path, const uint8_t* pData, size_t size)
	{
		const auto& textureItr = m_textures.find(path);
		if (textureItr != m_textures.end())
		{
			return textureItr->second;
		}
		else
		{
			Texture* newTexture = new Texture(pData, size);
			if (!newTexture->initialize(path))
			{
				delete newTexture;
				newTexture = m_defaultTexture;
			}
			m_textures.insert({ path, newTexture });
			return newTexture;
		}
	}

	Texture* ResourceManager::getColourTexturePtr(Colour colour)
	{
		const auto& textureItr = m_colourTextures.find(colour.getUnsignedInt());
		if (textureItr != m_colourTextures.end())
		{
			return textureItr->second;
		}
		else
		{
			Texture* newTexture = new Texture(colour);
			m_colourTextures.insert({ colour.getUnsignedInt(), newTexture });
			return newTexture;
		}
	}

	Material* ResourceManager::getMaterialPtr(std::string name)
	{
		const auto& materialItr = m_materials.find(name);
		if (materialItr != m_materials.end())
		{
			return materialItr->second;
		}
		else
		{
			Material* newMaterial = new Material();
			if (!newMaterial->initialise(name))
			{
				delete newMaterial;
				newMaterial = m_defaultMaterial;
			}
			m_materials.insert({ name, newMaterial });
			return newMaterial;
		}
	}

	VertexShader* ResourceManager::getVSPtr(std::string name)
	{
		const auto& vertexShaderItr = m_vertexShaders.find(name);
		if (vertexShaderItr != m_vertexShaders.end())
		{
			return vertexShaderItr->second;
		}
		else
		{
			VertexShader* newVertexShader = new VertexShader();
			std::wstring shaderPath = GraphicsHandler::it().getCompiledShaderFolder() + L"vs_" + utils::string_helpers::stringToWide(name) + L".cso";
			if (!newVertexShader->initialise(shaderPath, GraphicsHandler::it().getDefaultVSLayout(), GraphicsHandler::it().getDefaultVSLayoutSize()))
			{
				delete newVertexShader;
				newVertexShader = m_defaultVS;
			}
			m_vertexShaders.insert({ name, newVertexShader });
			return newVertexShader;
		}
	}

	PixelShader* ResourceManager::getPSPtr(std::string name)
	{
		const auto& pixelShaderItr = m_pixelShaders.find(name);
		if (pixelShaderItr != m_pixelShaders.end())
		{
			return pixelShaderItr->second;
		}
		else
		{
			PixelShader* newPixelShader = new PixelShader();
			std::wstring shaderPath = GraphicsHandler::it().getCompiledShaderFolder() + L"ps_" + utils::string_helpers::stringToWide(name) + L".cso";
			if (!newPixelShader->initialise(shaderPath))
			{
				delete newPixelShader;
				newPixelShader = m_defaultPS;
			}
			m_pixelShaders.insert({ name, newPixelShader });
			return newPixelShader;
		}
	}

	PixelShader* ResourceManager::getGBufferWritePSPtr(std::string name)
	{
		const auto& gBufferPixelShaderItr = m_gBufferWritePixelShaders.find(name);
		if (gBufferPixelShaderItr != m_gBufferWritePixelShaders.end())
		{
			return gBufferPixelShaderItr->second;
		}
		else
		{
			PixelShader* newGBufferWritePixelShader = new PixelShader();
			std::wstring shaderPath = GraphicsHandler::it().getCompiledShaderFolder() + L"ps_gbuf_w_" + utils::string_helpers::stringToWide(name) + L".cso";
			if (!newGBufferWritePixelShader->initialise(shaderPath))
			{
				delete newGBufferWritePixelShader;
				newGBufferWritePixelShader = m_defaultGBufferWritePS;
			}
			m_gBufferWritePixelShaders.insert({ name, newGBufferWritePixelShader });
			return newGBufferWritePixelShader;
		}
	}

	ComputeShader* ResourceManager::getCSPtr(std::string name)
	{
		const auto& computeShaderItr = m_computeShaders.find(name);
		if (computeShaderItr != m_computeShaders.end())
		{
			return computeShaderItr->second;
		}
		else
		{
			ComputeShader* newComputeShader = new ComputeShader();
			std::wstring shaderPath = GraphicsHandler::it().getCompiledShaderFolder() + L"cs_" + utils::string_helpers::stringToWide(name) + L".cso";
			if (!newComputeShader->initialise(shaderPath))
			{
				delete newComputeShader;
				newComputeShader = m_defaultCS;
			}
			m_computeShaders.insert({ name, newComputeShader });
			return newComputeShader;
		}
	}

	Model* ResourceManager::getDefaultModelPtr()
	{
		return m_defaultModel;
	}

	Texture* ResourceManager::getDefaultTexturePtr()
	{
		return m_defaultTexture;
	}

	Material* ResourceManager::getDefaultMaterialPtr()
	{
		return m_defaultMaterial;
	}

	VertexShader* ResourceManager::getDefaultVSPtr()
	{
		return m_defaultVS;
	}

	PixelShader* ResourceManager::getDefaultPSPtr()
	{
		return m_defaultPS;
	}

	PixelShader* ResourceManager::getDefaultGBufferWritePSPtr()
	{
		return m_defaultGBufferWritePS;
	}

	ComputeShader* ResourceManager::getDefaultCSPtr()
	{
		return m_defaultCS;
	}

	PixelShader* ResourceManager::getGBufferReadPSPtr()
	{
		return m_gBufferReadPS;
	}

	ResourceManager::ResourceManager() :
		m_defaultModel(nullptr),
		m_defaultTexture(nullptr),
		m_defaultMaterial(nullptr),

		m_defaultVS(nullptr),
		m_defaultPS(nullptr),
		m_defaultGBufferWritePS(nullptr),
		m_defaultCS(nullptr),

		m_gBufferReadPS(nullptr)
	{
	}
}
