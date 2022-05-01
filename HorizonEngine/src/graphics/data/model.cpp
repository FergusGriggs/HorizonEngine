
//Function implementations for the Model class

#include "model.h"

#include "resource_manager.h"

#include "../../physics/utils/collision_helpers.h"

#include "../graphics_handler.h"

#include <iostream>

namespace hrzn::gfx
{
	Model::Model() :
		m_meshes(),
		m_vertices(),
		m_indices(),
		m_currentNumVerts(0),

		m_useEmbeddedMaterials(false),
		m_debugDrawEnabled(false),

		m_filePath(""),
		m_directory(""),

		m_modelHitRadius(0.5f),
		m_boundingBox(),

		m_isGBufferCompatible(false)
	{
	}

	Model::~Model()
	{
		// Delete meshes
		for (auto* mesh : m_meshes)
		{
			delete mesh;
		}
		m_meshes.clear();

		// Delete mesh materials if required
		for (auto& meshMaterialIter : m_meshMaterials)
		{
			if (meshMaterialIter.second.m_deleteRequired)
			{
				delete meshMaterialIter.second.m_material;
			}
		}
		m_meshMaterials.clear();
	}

	void Model::draw(const XMMATRIX& modelMatrix, bool bindPSData) const
	{
		updateAlternatePerObjectCB();

		auto& perObjectCB = GraphicsHandler::it().getPerObjectCB();
		for (int i = 0; i < m_meshes.size(); i++)
		{
			perObjectCB.m_data.m_modelMatrix = m_meshes[i]->getTransformMatrix() * modelMatrix;
			//vertexShaderCB->data.modelMatrix = XMMatrixTranspose(vertexShaderCB->data.modelMatrix);

			perObjectCB.mapToGPU();

			m_meshes[i]->draw(m_isGBufferCompatible && GraphicsHandler::it().isUsingDeferredShading(), bindPSData);
		}

		if (m_debugDrawEnabled)
		{
			debugDraw(modelMatrix, bindPSData);
		}
	}

	void Model::updateAlternatePerObjectCB() const
	{
	}

	float Model::getHitRadius() const
	{
		return m_modelHitRadius;
	}

	void Model::drawRaw(bool useGBuffer, bool bindPSData)
	{
		for (int i = 0; i < m_meshes.size(); i++)
		{
			m_meshes[i]->draw(useGBuffer, bindPSData);
		}
	}

	const std::vector<Mesh*>& Model::getMeshes() const
	{
		return m_meshes;
	}

	const std::string& Model::getPath() const
	{
		return m_filePath;
	}

	const BoundingBox& Model::getBoundingBox() const
	{
		return m_boundingBox;
	}

	const std::vector<XMFLOAT3>& Model::getVertices() const
	{
		return m_vertices;
	}

	bool Model::rayInersectAllFaces(XMVECTOR rayOrigin, XMVECTOR rayDirection, float* rayDistance) const
	{
		for (int i = 0; i < m_indices.size(); i += 3)
		{
			// Load face vertices
			XMVECTOR vertex1 = XMLoadFloat3(&m_vertices[m_indices[i]]);
			XMVECTOR vertex2 = XMLoadFloat3(&m_vertices[m_indices[i + 1]]);
			XMVECTOR vertex3 = XMLoadFloat3(&m_vertices[m_indices[i + 2]]);

			if (physics::collision::rayFaceIntersect(vertex1, vertex2, vertex3, rayOrigin, rayDirection, rayDistance))
			{
				return true;
			}
		}

		return false;
	}

	bool Model::isGBufferCompatible() const
	{
		return m_isGBufferCompatible;
	}

	bool* Model::getDebugDrawPtr()
	{
		return &m_debugDrawEnabled;
	}

	void Model::loadModelMetaData(const std::string& filePath)
	{
		std::fstream metaDataFile(filePath);
		if (metaDataFile)
		{
			metaDataFile >> m_modelHitRadius;
			metaDataFile.close();
		}
	}

	bool Model::loadMeshMaterials(const std::string& filePath)
	{
		std::ifstream materialFile(filePath);

		if (materialFile)
		{
			std::string checkString;
			materialFile >> checkString;

			if (checkString != "#hrznmtls")
			{
				std::cout << "No '#hrznmtls' tag found in material link file at path '" + filePath + "' attempting to use embedded materials\n";
				//utils::ErrorLogger::log();
				materialFile.close();
				return false;
			}

			while (!materialFile.eof())
			{
				std::string meshName;
				materialFile >> meshName;

				std::string meshMaterialName;
				materialFile >> meshMaterialName;

				MeshMaterial meshMaterial;
				meshMaterial.m_material = ResourceManager::it().getMaterialPtr(meshMaterialName);
				meshMaterial.m_deleteRequired = false;

				m_meshMaterials.insert({ meshName, meshMaterial });
			}

			materialFile.close();

			return true;
		}
		else
		{
			//utils::ErrorLogger::log("Failed to load model material at path: " + filePath);
		}

		return false;
	}

	int Model::getTextureIndex(aiString* pString) const
	{
		assert(pString->length >= 2);
		return atoi((&pString->C_Str()[1]));
	}

	TextureStorageType Model::determineTextureStorageType(const aiScene* pScene, aiMaterial* pMaterial, unsigned int index, aiTextureType textureType) const
	{
		if (pMaterial->GetTextureCount(textureType) == 0)
		{
			return TextureStorageType::eNone;
		}

		aiString path;
		pMaterial->GetTexture(textureType, index, &path);
		std::string texturePath = path.C_Str();

		if (texturePath[0] == '*')
		{
			if (pScene->mTextures[0]->mHeight == 0)
			{
				return TextureStorageType::eEmbeddedIndexCompressed;
			}
			else {
				//assert("COMPRESSED TEXTURES ARE NOT SUPPORTED." && );
				return TextureStorageType::eEmbeddedIndexNonCompressed;
			}
		}
		if (auto pTex = pScene->GetEmbeddedTexture(texturePath.c_str()))
		{
			if (pTex->mHeight == 0)
			{
				return TextureStorageType::eEmbeddedCompressed;
			}
			else {
				//assert("COMPRESSED TEXTURES ARE NOT SUPPORTED." && );
				return TextureStorageType::eEmbeddedNonCompressed;
			}
		}
		if (texturePath.find('.') != std::string::npos)
		{
			return TextureStorageType::eDisk;
		}

		return TextureStorageType::eNone;
	}

	void Model::loadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene, std::vector<Texture*>& materialTextures)
	{
		TextureStorageType storageType = TextureStorageType::eInvalid;
		unsigned int textureCount = pMaterial->GetTextureCount(textureType);

		if (textureCount == 0)
		{
			storageType = TextureStorageType::eNone;
			aiColor3D aiColour(0.0f, 0.0f, 0.0f);

			switch (textureType)
			{
			case aiTextureType_DIFFUSE:
			{
				pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColour);

				if (aiColour.IsBlack())
				{
					materialTextures.push_back(ResourceManager::it().getColourTexturePtr(colours::sc_defaultAlbedo));
					return;
				}
				materialTextures.push_back(ResourceManager::it().getColourTexturePtr(Colour((BYTE)aiColour.r * 255, (BYTE)aiColour.g * 255, (BYTE)aiColour.b * 255)));
				return;
			}
			case aiTextureType_SPECULAR:
				materialTextures.push_back(ResourceManager::it().getColourTexturePtr(colours::sc_defaultRoughness));
				return;

			case aiTextureType_NORMALS:
				materialTextures.push_back(ResourceManager::it().getColourTexturePtr(colours::sc_defaultNormal));
				return;

			case aiTextureType_DISPLACEMENT:
				materialTextures.push_back(ResourceManager::it().getColourTexturePtr(colours::sc_defaultDepth));
				return;
			}
		}
		else
		{
			for (UINT i = 0; i < textureCount; i++)
			{
				aiString path;
				pMaterial->GetTexture(textureType, i, &path);

				TextureStorageType storageType = determineTextureStorageType(pScene, pMaterial, i, textureType);

				switch (storageType)
				{
				case TextureStorageType::eDisk:
				{
					std::string fileName = m_directory + "/" + path.C_Str();
					materialTextures.push_back(ResourceManager::it().getTexturePtr(fileName));
					break;
				}
				case TextureStorageType::eEmbeddedCompressed:
				{
					const aiTexture* pTexture = pScene->GetEmbeddedTexture(path.C_Str());
					std::string key = m_directory + "/" + path.C_Str();
					materialTextures.push_back(ResourceManager::it().getTexturePtr(key, reinterpret_cast<uint8_t*>(pTexture->pcData), pTexture->mWidth));
					break;
				}
				case TextureStorageType::eEmbeddedIndexCompressed:
				{
					int index = getTextureIndex(&path);
					std::string key = m_directory + "/" + path.C_Str();
					materialTextures.push_back(ResourceManager::it().getTexturePtr(key, reinterpret_cast<uint8_t*>(pScene->mTextures[index]->pcData), pScene->mTextures[index]->mWidth));
					break;
				}
				}
			}
			return;
		}

		if (materialTextures.size() == 0)
		{
			materialTextures.push_back(ResourceManager::it().getColourTexturePtr(colours::sc_unhandledTextureColour));
			return;
		}

		return;
	}
}
