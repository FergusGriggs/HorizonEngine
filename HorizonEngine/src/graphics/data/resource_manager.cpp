#include "resource_manager.h"

#include "../graphics_handler.h"

#include "../../utils/string_helpers.h"
#include "../data/vertex_types.h"

#include "../data/skinned_model.h"

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
		m_defaultModel = getModelPtr<FancyLitVertex>("res/models/engine/test/horizon_statue.obj");

		m_defaultVS = getVSPtr("default");
		m_defaultPS = getPSPtr("standard_default");
		m_defaultGBufferWritePS = getGBufferWritePSPtr("default");
		m_defaultCS = nullptr;

		m_gBufferReadPS = getPSPtr("gbuf_r_default");

		return true;
	}

	Model* ResourceManager::getSkinnedModelPtr(const std::string& path)
	{
		const auto& modelItr = m_models.find(path);
		if (modelItr != m_models.end())
		{
			return modelItr->second;
		}
		else
		{
			SkinnedModel* loadedModel = new SkinnedModel();
			if (!loadedModel->initialise(path))
			{
				delete loadedModel;
				loadedModel = nullptr;
			}
			m_models.insert({ path, loadedModel });
			return loadedModel;
		}
	}

	Texture* ResourceManager::getTexturePtr(const std::string& path)
	{
		const auto& textureItr = m_textures.find(path);
		if (textureItr != m_textures.end())
		{
			return textureItr->second;
		}
		else
		{
			Texture* loadedTexture = new Texture();
			if (!loadedTexture->initialise(path))
			{
				delete loadedTexture;
				loadedTexture = m_defaultTexture;
			}
			m_textures.insert({ path, loadedTexture });
			return loadedTexture;
		}
	}

	Texture* ResourceManager::getTexturePtr(const std::string& path, const uint8_t* pData, size_t size)
	{
		const auto& textureItr = m_textures.find(path);
		if (textureItr != m_textures.end())
		{
			return textureItr->second;
		}
		else
		{
			Texture* newTexture = new Texture(pData, size);
			if (!newTexture->initialise(path))
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

	Material* ResourceManager::getMaterialPtr(const std::string& name)
	{
		const auto& materialItr = m_materials.find(name);
		if (materialItr != m_materials.end())
		{
			return materialItr->second;
		}
		else
		{
			Material* newMaterial = new Material();
			if (!newMaterial->initialiseFromName(name))
			{
				delete newMaterial;
				newMaterial = m_defaultMaterial;
			}
			m_materials.insert({ name, newMaterial });
			return newMaterial;
		}
	}

	VertexShader* ResourceManager::getVSPtr(const std::string& name)
	{
		const auto& vertexShaderItr = m_vertexShaders.find(name);
		if (vertexShaderItr != m_vertexShaders.end())
		{
			return vertexShaderItr->second;
		}
		else
		{
			VertexShader* newVertexShader = new VertexShader();
			//std::string shaderPath =  "res/shader/vertex/vs_" + name + ".hlsl";
			std::string shaderPath = GraphicsHandler::it().getCompiledShaderFolder() + "vs_" + name + ".cso";
			if (!newVertexShader->loadCompiledInit(shaderPath.c_str()))
			{
				delete newVertexShader;
				newVertexShader = m_defaultVS;
			}
			m_vertexShaders.insert({ name, newVertexShader });
			return newVertexShader;
		}
	}

	PixelShader* ResourceManager::getPSPtr(const std::string& name)
	{
		const auto& pixelShaderItr = m_pixelShaders.find(name);
		if (pixelShaderItr != m_pixelShaders.end())
		{
			return pixelShaderItr->second;
		}
		else
		{
			PixelShader* newPixelShader = new PixelShader();
			//std::string shaderPath = "res/shader/pixel/ps_" + name + ".hlsl";
			std::string shaderPath = GraphicsHandler::it().getCompiledShaderFolder() + "ps_" + name + ".cso";
			if (!newPixelShader->loadCompiledInit(shaderPath.c_str()))
			{
				delete newPixelShader;
				newPixelShader = m_defaultPS;
			}
			m_pixelShaders.insert({ name, newPixelShader });
			return newPixelShader;
		}
	}

	PixelShader* ResourceManager::getGBufferWritePSPtr(const std::string& name)
	{
		const auto& gBufferPixelShaderItr = m_gBufferWritePixelShaders.find(name);
		if (gBufferPixelShaderItr != m_gBufferWritePixelShaders.end())
		{
			return gBufferPixelShaderItr->second;
		}
		else
		{
			PixelShader* newGBufferWritePixelShader = new PixelShader();
			//std::string shaderPath = "res/shader/pixel/ps_gbuf_w_" + name + ".hlsl";
			std::string shaderPath = GraphicsHandler::it().getCompiledShaderFolder() + "ps_gbuf_w_" + name + ".cso";
			if (!newGBufferWritePixelShader->loadCompiledInit(shaderPath.c_str()))
			{
				delete newGBufferWritePixelShader;
				newGBufferWritePixelShader = m_defaultGBufferWritePS;
			}
			m_gBufferWritePixelShaders.insert({ name, newGBufferWritePixelShader });
			return newGBufferWritePixelShader;
		}
	}

	ComputeShader* ResourceManager::getCSPtr(const std::string& name)
	{
		const auto& computeShaderItr = m_computeShaders.find(name);
		if (computeShaderItr != m_computeShaders.end())
		{
			return computeShaderItr->second;
		}
		else
		{
			ComputeShader* newComputeShader = new ComputeShader();
			//std::string shaderPath = "res/shader/compute/cs_" + name + ".hlsl";
			std::string shaderPath = GraphicsHandler::it().getCompiledShaderFolder() + "cs_" + name + ".cso";
			if (!newComputeShader->loadCompiledInit(shaderPath.c_str()))
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
