
//Function implementations for the Model class

#include "model.h"

#include "resource_manager.h"

#include "../../physics/utils/collision_helpers.h"

#include "../graphics_handler.h"

namespace hrzn::gfx
{
	Model::Model() :
		m_meshes(),
		m_vertices(),
		m_indices(),
		m_currentNumVerts(0),

		m_filePath(""),
		m_directory(""),

		m_modelHitRadius(0.5f),
		m_boundingBox(),

		m_isGBufferCompatible(false)
	{
	}

	bool Model::initialize(const std::string& filePath)
	{
		m_filePath = filePath;

		try
		{
			if (!loadModel(filePath))
			{
				return false;
			}
		}
		catch (utils::COMException& exception)
		{
			utils::ErrorLogger::log(exception);
			return false;
		}

		return true;
	}

	void Model::draw(const XMMATRIX& modelMatrix, ConstantBuffer<PerObjectCB>* perObjectCB, bool bindPSData) const
	{
		for (int i = 0; i < m_meshes.size(); i++)
		{
			perObjectCB->m_data.m_modelMatrix = m_meshes[i].getTransformMatrix() * modelMatrix;
			//vertexShaderCB->data.modelMatrix = XMMatrixTranspose(vertexShaderCB->data.modelMatrix);

			perObjectCB->mapToGPU();

			m_meshes[i].draw(m_isGBufferCompatible && GraphicsHandler::it().isUsingDeferredShading(), bindPSData);
		}
	}

	float Model::getHitRadius() const
	{
		return m_modelHitRadius;
	}

	void Model::drawRaw(bool useGBuffer, bool bindPSData)
	{
		for (int i = 0; i < m_meshes.size(); i++)
		{
			m_meshes[i].draw(useGBuffer, bindPSData);
		}
	}

	const std::vector<Mesh>& Model::getMeshes() const
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

	bool Model::loadModel(const std::string& filePath)
	{
		m_directory = utils::string_helpers::getDirectoryFromPath(filePath);

		std::string meshMaterialFilePath = utils::string_helpers::changeFileExtension(filePath, "mtl");
		loadMeshMaterials(meshMaterialFilePath);

		Assimp::Importer importer;

		const aiScene* pScene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_ConvertToLeftHanded);

		//aiProcess_ConvertToLeftHanded
		// v
		//aiProcess_MakeLeftHanded
		//aiProcess_FlipUVs
		//aiProcess_FlipWindingOrder

		if (pScene == nullptr)
		{
			return false;
		}

		m_isGBufferCompatible = true;

		processNode(pScene->mRootNode, pScene, XMMatrixIdentity());// * XMMatrixScaling(0.025f, 0.025f, 0.025f)

		if (m_vertices.empty())
		{
			utils::ErrorLogger::log("Model at path '" + filePath + "' failed to load as it had no vertices");
			return false;
		}

		BoundingBox::CreateFromPoints(m_boundingBox, m_vertices.size(), &(m_vertices.at(0)), sizeof(XMFLOAT3));

		loadModelMetaData(filePath.substr(0, filePath.length() - 4) + "_meta.txt"); // Remove model file extension, add '_meta.txt'

		return true;
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
				utils::ErrorLogger::log("No '#hrznmtls' tag found in material link file at path '" + filePath + "'");
				materialFile.close();
				return false;
			}

			while (!materialFile.eof())
			{
				std::string meshName;
				materialFile >> meshName;

				std::string meshMaterialName;
				materialFile >> meshMaterialName;

				m_meshMaterials.insert(std::make_pair(meshName, ResourceManager::it().getMaterialPtr(meshMaterialName)));
			}

			materialFile.close();

			return true;
		}
		else
		{
			utils::ErrorLogger::log("Failed to load model material at path: " + filePath);
		}

		return false;
	}

	void Model::processNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix)
	{
		XMMATRIX nodeTransformationMatrix = XMMatrixTranspose(XMMATRIX(&node->mTransformation.a1)) * parentTransformMatrix;

		for (UINT i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			Material* meshMaterial = nullptr;
			const auto& meshMaterialsIterator = m_meshMaterials.find(node->mName.C_Str());
			if (meshMaterialsIterator != m_meshMaterials.end())
			{
				meshMaterial = meshMaterialsIterator->second;
			}
			else
			{
				meshMaterial = ResourceManager::it().getDefaultMaterialPtr();
			}

			if (!meshMaterial->isGBufferCompatible())
			{
				m_isGBufferCompatible = false;
			}

			m_meshes.push_back(processMesh(mesh, meshMaterial, nodeTransformationMatrix));
		}

		for (UINT i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene, nodeTransformationMatrix);
		}
	}

	Mesh Model::processMesh(aiMesh* mesh, Material* material, const XMMATRIX& transformMatrix)
	{
		std::vector<Vertex> meshVertices;
		std::vector<DWORD> indices;

		for (UINT i = 0; i < mesh->mNumVertices; i++)
		{

			Vertex vertex;

			vertex.m_pos.x = mesh->mVertices[i].x;
			vertex.m_pos.y = mesh->mVertices[i].y;
			vertex.m_pos.z = mesh->mVertices[i].z;

			vertex.m_normal.x = mesh->mNormals[i].x;
			vertex.m_normal.y = mesh->mNormals[i].y;
			vertex.m_normal.z = mesh->mNormals[i].z;

			if (mesh->mTextureCoords[0])
			{
				vertex.m_tangent.x = mesh->mTangents[i].x;
				vertex.m_tangent.y = mesh->mTangents[i].y;
				vertex.m_tangent.z = mesh->mTangents[i].z;

				vertex.m_bitangent.x = mesh->mBitangents[i].x;
				vertex.m_bitangent.y = mesh->mBitangents[i].y;
				vertex.m_bitangent.z = mesh->mBitangents[i].z;


				vertex.m_texCoord.x = (float)mesh->mTextureCoords[0][i].x;
				vertex.m_texCoord.y = (float)mesh->mTextureCoords[0][i].y;
			}

			meshVertices.push_back(vertex);
			m_vertices.push_back(vertex.m_pos);
		}

		for (UINT i = 0; i < mesh->mNumFaces; i++)
		{

			aiFace face = mesh->mFaces[i];

			for (UINT j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
				m_indices.push_back(m_currentNumVerts + face.mIndices[j]);
			}
		}

		m_currentNumVerts = static_cast<int>(m_vertices.size());

		/*std::vector<Texture*> textures;
		aiMaterial* aiMaterial = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture*> diffuseTextures = loadMaterialTextures(aiMaterial, aiTextureType::aiTextureType_DIFFUSE, scene);
		textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());

		std::vector<Texture*> specularTextures = loadMaterialTextures(aiMaterial, aiTextureType::aiTextureType_SPECULAR, scene);
		textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());

		std::vector<Texture*> normalTextures = loadMaterialTextures(aiMaterial, aiTextureType::aiTextureType_NORMALS, scene);
		textures.insert(textures.end(), normalTextures.begin(), normalTextures.end());

		std::vector<Texture*> depthTextures = loadMaterialTextures(aiMaterial, aiTextureType::aiTextureType_DISPLACEMENT, scene);
		textures.insert(textures.end(), depthTextures.begin(), depthTextures.end());*/

		return Mesh(meshVertices, indices, material, transformMatrix);
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

	std::vector<Texture*> Model::loadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene)
	{
		std::vector<Texture*> materialTextures;
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
					return materialTextures;
				}
				materialTextures.push_back(ResourceManager::it().getColourTexturePtr(Colour((BYTE)aiColour.r * 255, (BYTE)aiColour.g * 255, (BYTE)aiColour.b * 255)));
				return materialTextures;
			}
			case aiTextureType_SPECULAR:
				materialTextures.push_back(ResourceManager::it().getColourTexturePtr(colours::sc_defaultRoughness));
				return materialTextures;

			case aiTextureType_NORMALS:
				materialTextures.push_back(ResourceManager::it().getColourTexturePtr(colours::sc_defaultNormal));
				return materialTextures;

			case aiTextureType_DISPLACEMENT:
				materialTextures.push_back(ResourceManager::it().getColourTexturePtr(colours::sc_defaultDepth));
				return materialTextures;
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
			return materialTextures;
		}

		if (materialTextures.size() == 0)
		{
			materialTextures.push_back(ResourceManager::it().getColourTexturePtr(colours::sc_unhandledTextureColour));
			return materialTextures;
		}

		return materialTextures;
	}
}
