#pragma once

#include "model.h"

#include "resource_manager.h"

namespace hrzn::gfx
{
	template <class VertexType>
	inline bool Model::initialise(const std::string& filePath)
	{
		m_filePath = filePath;

		try
		{
			if (!loadModel<VertexType>(filePath))
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

	template <class VertexType>
	inline bool Model::loadModel(const std::string& filePath)
	{
		m_directory = utils::string_helpers::getDirectoryFromPath(filePath);

		std::string meshMaterialFilePath = utils::string_helpers::changeFileExtension(filePath, "mtl");
		if (!loadMeshMaterials(meshMaterialFilePath))
		{
			m_useEmbeddedMaterials = true;
		}

		Assimp::Importer importer;

		//aiProcess_ConvertToLeftHanded
		// v
		//aiProcess_MakeLeftHanded
		//aiProcess_FlipUVs
		//aiProcess_FlipWindingOrder

		const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_ConvertToLeftHanded);
		if (scene == nullptr)
		{
			return false;
		}

		m_isGBufferCompatible = true;

		processNode<VertexType>(scene->mRootNode, scene, XMMatrixIdentity());// * XMMatrixScaling(0.025f, 0.025f, 0.025f)

		if (m_vertices.empty())
		{
			utils::ErrorLogger::log("Model at path '" + filePath + "' failed to load as it had no vertices");
			return false;
		}

		postSceneBasicParse(scene);

		BoundingBox::CreateFromPoints(m_boundingBox, m_vertices.size(), &(m_vertices.at(0)), sizeof(XMFLOAT3));

		loadModelMetaData(filePath.substr(0, filePath.length() - 4) + "_meta.txt"); // Remove model file extension, add '_meta.txt'

		return true;
	}

	template <class VertexType>
	inline void Model::processNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix)
	{
		XMMATRIX nodeTransformationMatrix = XMMatrixTranspose(XMMATRIX(&node->mTransformation.a1)) * parentTransformMatrix;

		for (UINT i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			Material* meshMaterialPtr = nullptr;

			if (std::string(node->mName.C_Str()) == std::string("head"))
			{
				int cat = 12;
			}

			if (m_useEmbeddedMaterials)
			{
				MeshMaterial meshMaterial;
				meshMaterial.m_material = new Material();
				meshMaterial.m_deleteRequired = true;

				aiMaterial* aiMaterial = scene->mMaterials[mesh->mMaterialIndex];
				std::vector<MaterialTexture> finalMaterialTextures;

				std::vector<Texture*> albedoTextures;
				loadMaterialTextures(aiMaterial, aiTextureType::aiTextureType_DIFFUSE, scene, albedoTextures);
				if (!albedoTextures.empty())
				{
					finalMaterialTextures.push_back(MaterialTexture(MaterialTextureType::eAlbedo, albedoTextures[0]));
				}

				std::vector<Texture*> specularTextures;
				loadMaterialTextures(aiMaterial, aiTextureType::aiTextureType_SPECULAR, scene, specularTextures);
				if (!specularTextures.empty())
				{
					finalMaterialTextures.push_back(MaterialTexture(MaterialTextureType::eRoughness, specularTextures[0]));
				}

				std::vector<Texture*> normalTextures;
				loadMaterialTextures(aiMaterial, aiTextureType::aiTextureType_NORMALS, scene, normalTextures);
				if (!normalTextures.empty())
				{
					finalMaterialTextures.push_back(MaterialTexture(MaterialTextureType::eNormal, normalTextures[0]));
				}

				std::vector<Texture*> displacementTextures;
				loadMaterialTextures(aiMaterial, aiTextureType::aiTextureType_DISPLACEMENT, scene, displacementTextures);
				if (!displacementTextures.empty())
				{
					finalMaterialTextures.push_back(MaterialTexture(MaterialTextureType::eDepth, displacementTextures[0]));
				}
				
				meshMaterial.m_material->initialiseFromTextures(finalMaterialTextures);

				m_meshMaterials.insert({ std::string(node->mName.C_Str()), meshMaterial });

				meshMaterialPtr = meshMaterial.m_material;
			}
			else
			{
				const auto& meshMaterialsIterator = m_meshMaterials.find(node->mName.C_Str());
				if (meshMaterialsIterator != m_meshMaterials.end())
				{
					meshMaterialPtr = meshMaterialsIterator->second.m_material;
				}
				else
				{
					meshMaterialPtr = ResourceManager::it().getDefaultMaterialPtr();
				}
			}

			if (!meshMaterialPtr->isGBufferCompatible())
			{
				m_isGBufferCompatible = false;
			}

			m_meshes.push_back(processMesh<VertexType>(mesh, meshMaterialPtr, nodeTransformationMatrix));
		}

		for (UINT i = 0; i < node->mNumChildren; i++)
		{
			processNode<VertexType>(node->mChildren[i], scene, nodeTransformationMatrix);
		}
	}

	template <class VertexType>
	inline Mesh* Model::processMesh(aiMesh* mesh, Material* material, const XMMATRIX& transformMatrix)
	{
		std::vector<VertexType> meshVertices;
		std::vector<DWORD> indices;

		for (UINT i = 0; i < mesh->mNumVertices; i++)
		{
			VertexType vertex;
			vertex.fillUsingAssimpMesh(mesh, i);

			meshVertices.push_back(vertex);
			m_vertices.push_back(vertex.m_pos.getAsXMFLOAT3());
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

		std::string meshName = mesh->mName.C_Str();

		return new Mesh(meshName, meshVertices, indices, material);
	}
}
