
//Handles the loading of 3D models with the help of Assimp, all vertex and index data is stored in meshes in an std::vector

#pragma once

#include <fstream>

#include <DirectXCollision.h>

#include <assimp/material.h>

#include "mesh.h"
#include "texture.h"
#include "../buffers/constant_buffer.h"

using namespace DirectX;

namespace hrzn::gfx
{
	class Model
	{
	public:
		Model();
		~Model();

		template<class VertexType>
		bool initialise(const std::string& filePath);

		void draw(const XMMATRIX& modelMatrix, gfx::ConstantBuffer<PerObjectCB>* perObjectCB, bool bindPSData = true) const;

		void drawRaw(bool useGBuffer, bool bindPSData = true);

		const std::vector<Mesh*>&    getMeshes() const;

		const std::string&           getPath() const;
		const std::vector<XMFLOAT3>& getVertices() const;

		float              getHitRadius() const;
		const BoundingBox& getBoundingBox() const;
		bool               rayInersectAllFaces(XMVECTOR rayOrigin, XMVECTOR rayDirection, float* rayDistance) const;

		bool               isGBufferCompatible() const;

	protected:
		template<class VertexType>
		bool         loadModel(const std::string& filePath);
		virtual void postSceneBasicParse(const aiScene* scene) {};

	private:
		template<class VertexType>
		void processNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix);

		template<class VertexType>
		Mesh* processMesh(aiMesh* mesh,  Material* material, const XMMATRIX& transformMatrix);

		void loadModelMetaData(const std::string& filePath);

		bool loadMeshMaterials(const std::string& filePath);

		int                   getTextureIndex(aiString* pString) const;
		TextureStorageType    determineTextureStorageType(const aiScene* pScene, aiMaterial* pMaterial, unsigned int index, aiTextureType textureType) const;
		void                  loadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene, std::vector<Texture*>& materialTextures);

	protected:
		std::vector<Mesh*>    m_meshes;
		std::vector<XMFLOAT3> m_vertices;
		std::vector<DWORD>    m_indices;
		int                   m_currentNumVerts;

		bool                  m_useEmbeddedMaterials;

		struct MeshMaterial
		{
			Material* m_material = nullptr;
			bool      m_deleteRequired = false;
		};

		std::unordered_map<std::string, MeshMaterial> m_meshMaterials;

		std::string           m_filePath;
		std::string           m_directory;

		float                 m_modelHitRadius;
		BoundingBox           m_boundingBox;

		bool m_isGBufferCompatible;
	};
}

#include "model.inl"
