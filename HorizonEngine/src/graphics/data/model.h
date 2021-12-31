
//Handles the loading of 3D models with the help of Assimp, all vertex and index data is stored in meshes in an std::vector

#pragma once

#include <DirectXCollision.h>

#include <fstream>

#include "mesh.h"

using namespace DirectX;

namespace hrzn::gfx
{
	class Model
	{
	public:
		Model();

		bool initialize(const std::string& filePath);

		void draw(const XMMATRIX& modelMatrix, ConstantBuffer<PerObjectCB>* perObjectCB, bool bindPSData = true) const;

		void drawRaw(bool useGBuffer, bool bindPSData = true);

		const std::vector<Mesh>&     getMeshes() const;

		const std::string&           getPath() const;
		const std::vector<XMFLOAT3>& getVertices() const;

		float              getHitRadius() const;
		const BoundingBox& getBoundingBox() const;
		bool               rayInersectAllFaces(XMVECTOR rayOrigin, XMVECTOR rayDirection, float* rayDistance) const;

		bool               isGBufferCompatible() const;

	private:
		bool loadModel(const std::string& filePath);
		void processNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix);
		Mesh processMesh(aiMesh* mesh,  Material* material, const XMMATRIX& transformMatrix);
		void loadModelMetaData(const std::string& filePath);

		bool loadMeshMaterials(const std::string& filePath);

		int                   getTextureIndex(aiString* pString) const;
		TextureStorageType    determineTextureStorageType(const aiScene* pScene, aiMaterial* pMaterial, unsigned int index, aiTextureType textureType) const;
		std::vector<Texture*> loadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);

	private:
		std::vector<Mesh>     m_meshes;
		std::vector<XMFLOAT3> m_vertices;
		std::vector<DWORD>    m_indices;
		int                   m_currentNumVerts;

		std::unordered_map<std::string, Material*> m_meshMaterials;

		std::string           m_filePath;
		std::string           m_directory;

		float                 m_modelHitRadius;
		BoundingBox           m_boundingBox;

		bool m_isGBufferCompatible;
	};
}
