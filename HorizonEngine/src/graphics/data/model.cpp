
//Function implementations for the Model class

#include "model.h"

#include "resource_manager.h";

namespace hrzn::gfx
{
	Model::Model() :
		m_meshes(),
		m_vertices(),
		m_indices(),
		m_currentNumVerts(0),

		m_device(nullptr),
		m_deviceContext(nullptr),

		m_filePath(""),
		m_directory(""),

		m_modelHitRadius(0.5f),
		m_boundingBox()
	{
	}

	bool Model::initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		m_filePath = filePath;
		m_device = device;
		m_deviceContext = deviceContext;

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

	void Model::draw(const XMMATRIX& modelMatrix, const XMMATRIX& viewProjectionMatrix, ConstantBuffer<VertexShaderCB>* vertexShaderCB, bool bindTextures)
	{
		m_deviceContext->VSSetConstantBuffers(0, 1, vertexShaderCB->getAddressOf());

		for (int i = 0; i < m_meshes.size(); i++)
		{
			vertexShaderCB->m_data.m_modelViewProjectionMatrix = m_meshes[i].getTransformMatrix() * modelMatrix * viewProjectionMatrix;
			//vertexShaderCB->data.modelViewProjectionMatrix = XMMatrixTranspose(vertexShaderCB->data.modelViewProjectionMatrix);

			vertexShaderCB->m_data.m_modelMatrix = m_meshes[i].getTransformMatrix() * modelMatrix;
			//vertexShaderCB->data.modelMatrix = XMMatrixTranspose(vertexShaderCB->data.modelMatrix);

			vertexShaderCB->mapToGPU();

			m_meshes[i].draw(bindTextures);
		}
	}

	float Model::getHitRadius()
	{
		return m_modelHitRadius;
	}

	std::string Model::getPath()
	{
		return m_filePath;
	}

	BoundingBox Model::getBoundingBox()
	{
		return m_boundingBox;
	}

	std::vector<XMFLOAT3>* Model::getVertices()
	{
		return &m_vertices;
	}

	bool Model::rayInersect(XMVECTOR rayOrigin, XMVECTOR rayDirection, float* rayDistance)
	{
		for (int i = 0; i < m_indices.size(); i += 3)
		{
			// Load vertex positions
			XMVECTOR vertex1 = XMLoadFloat3(&m_vertices[m_indices[i]]);
			XMVECTOR vertex2 = XMLoadFloat3(&m_vertices[m_indices[i + 1]]);
			XMVECTOR vertex3 = XMLoadFloat3(&m_vertices[m_indices[i + 2]]);

			// Face normal
			XMVECTOR faceNormal = XMVector3Normalize(XMVector3Cross(vertex2 - vertex1, vertex3 - vertex1));


			// Plane intersect point
			XMVECTOR diff = rayOrigin - vertex1;
			XMVECTOR planeIntersectPoint = (diff + vertex1) + rayDirection * (-XMVector3Dot(diff, faceNormal) / XMVector3Dot(rayDirection, faceNormal));

			// Check if the ray could hit the face
			if (XMVectorGetX(XMVector3Dot(planeIntersectPoint - rayOrigin, rayDirection)) >= 0.0)
			{
				// Work out barrycentric coordinates to check if point is in face
				XMVECTOR v0 = vertex2 - vertex1;
				XMVECTOR v1 = vertex3 - vertex1;
				XMVECTOR v2 = planeIntersectPoint - vertex1;

				float d00 = XMVectorGetX(XMVector3Dot(v0, v0));
				float d01 = XMVectorGetX(XMVector3Dot(v0, v1));
				float d11 = XMVectorGetX(XMVector3Dot(v1, v1));
				float d20 = XMVectorGetX(XMVector3Dot(v2, v0));
				float d21 = XMVectorGetX(XMVector3Dot(v2, v1));
				float denom = d00 * d11 - d01 * d01;
				float v = (d11 * d20 - d01 * d21) / denom;
				float w = (d00 * d21 - d01 * d20) / denom;
				float u = 1.0f - v - w;

				if ((u >= 0.0f) && (v >= 0.0f) && (u + v <= 1.0f))
				{
					*rayDistance = XMVectorGetX(XMVector3Length(rayOrigin - planeIntersectPoint));
					return true;
				}
			}
		}

		return false;
	}

	bool Model::loadModel(const std::string& filePath)
	{
		m_directory = utils::string_helpers::getDirectoryFromPath(filePath);

		Assimp::Importer importer;

		const aiScene* pScene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_ConvertToLeftHanded);

		if (pScene == nullptr)
		{
			return false;
		}

		processNode(pScene->mRootNode, pScene, XMMatrixIdentity());// * XMMatrixScaling(0.025f, 0.025f, 0.025f)

		BoundingBox::CreateFromPoints(m_boundingBox, m_vertices.size(), &(m_vertices.at(0)), sizeof(XMFLOAT3));

		loadModelMetaData(filePath.substr(0, filePath.length() - 4) + "_meta.txt");//remove model file extension, add '_meta.txt'

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

	void Model::processNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix)
	{
		XMMATRIX nodeTransformationMatrix = XMMatrixTranspose(XMMATRIX(&node->mTransformation.a1)) * parentTransformMatrix;

		for (UINT i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_meshes.push_back(processMesh(mesh, scene, nodeTransformationMatrix));
		}

		for (UINT i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene, nodeTransformationMatrix);
		}
	}

	Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& transformMatrix)
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

		m_currentNumVerts = m_vertices.size();

		std::vector<Texture*> textures;
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture*> diffuseTextures = loadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, scene);
		textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());

		std::vector<Texture*> specularTextures = loadMaterialTextures(material, aiTextureType::aiTextureType_SPECULAR, scene);
		textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());

		std::vector<Texture*> normalTextures = loadMaterialTextures(material, aiTextureType::aiTextureType_HEIGHT, scene);
		textures.insert(textures.end(), normalTextures.begin(), normalTextures.end());

		std::vector<Texture*> depthTextures = loadMaterialTextures(material, aiTextureType::aiTextureType_DISPLACEMENT, scene);
		textures.insert(textures.end(), depthTextures.begin(), depthTextures.end());

		return Mesh(m_device, m_deviceContext, meshVertices, indices, textures, transformMatrix);
	}

	int Model::getTextureIndex(aiString* pString)
	{
		assert(pString->length >= 2);
		return atoi((&pString->C_Str()[1]));
	}

	TextureStorageType Model::determineTextureStorageType(const aiScene* pScene, aiMaterial* pMaterial, unsigned int index, aiTextureType textureType)
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
					materialTextures.push_back(ResourceManager::it().getColourTexturePtr("UNLOADED_DIFFUSE", colours::sc_unloadedTextureColour, textureType));
					return materialTextures;
				}
				std::string key = std::to_string(static_cast<int>(aiColour.r * 255)) + "|" + std::to_string(static_cast<int>(aiColour.g * 255)) + "|" + std::to_string(static_cast<int>(aiColour.b * 255));
				materialTextures.push_back(ResourceManager::it().getColourTexturePtr(key, Colour(aiColour.r * 255, aiColour.g * 255, aiColour.b * 255), textureType));
				return materialTextures;
			}
			case aiTextureType_SPECULAR:
				materialTextures.push_back(ResourceManager::it().getColourTexturePtr("UNLOADED_SPECULAR", colours::sc_unloadedSpecularTextureColour, textureType));
				return materialTextures;

				return materialTextures;

			case aiTextureType_HEIGHT:
				materialTextures.push_back(ResourceManager::it().getColourTexturePtr("UNLOADED_NORMAL", colours::sc_unloadedNormalTextureColour, textureType));
				return materialTextures;

			case aiTextureType_DISPLACEMENT:
				materialTextures.push_back(ResourceManager::it().getColourTexturePtr("UNLOADED_DEPTH", colours::sc_unloadedDepthTextureColour, textureType));
				return materialTextures;
			}
		}
		else {
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
					materialTextures.push_back(ResourceManager::it().getTexturePtr(fileName, textureType));
					break;
				}
				case TextureStorageType::eEmbeddedCompressed:
				{
					const aiTexture* pTexture = pScene->GetEmbeddedTexture(path.C_Str());
					std::string key = m_directory + "/" + path.C_Str();
					materialTextures.push_back(ResourceManager::it().getTexturePtr(key, reinterpret_cast<uint8_t*>(pTexture->pcData), pTexture->mWidth, textureType));
					break;
				}
				case TextureStorageType::eEmbeddedIndexCompressed:
				{
					int index = getTextureIndex(&path);
					std::string key = m_directory + "/" + path.C_Str();
					materialTextures.push_back(ResourceManager::it().getTexturePtr(key, reinterpret_cast<uint8_t*>(pScene->mTextures[index]->pcData), pScene->mTextures[index]->mWidth, textureType));
					break;
				}
				}
			}
			return materialTextures;
		}

		if (materialTextures.size() == 0)
		{
			materialTextures.push_back(ResourceManager::it().getColourTexturePtr("UNHANDLED", colours::sc_unhandledTextureColour, textureType));
			return materialTextures;
		}
	}
}
