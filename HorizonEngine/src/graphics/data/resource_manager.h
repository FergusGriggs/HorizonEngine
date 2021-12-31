#pragma once

#include <unordered_map>

#include "model.h"
#include "texture.h"
#include "material.h"
#include "../shaders/shader.h"

namespace hrzn::gfx
{
	class ResourceManager
	{
	public:
		static ResourceManager& it();

		bool initialise();

		Model*    getModelPtr(std::string path);

		Texture*  getTexturePtr(std::string path);
		Texture*  getTexturePtr(std::string path, const uint8_t* pData, size_t size);
		Texture*  getColourTexturePtr(Colour colour);

		Material* getMaterialPtr(std::string name);

		VertexShader*  getVSPtr(std::string name);
		PixelShader*   getPSPtr(std::string name);
		PixelShader*   getGBufferWritePSPtr(std::string name);
		ComputeShader* getCSPtr(std::string name);

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
