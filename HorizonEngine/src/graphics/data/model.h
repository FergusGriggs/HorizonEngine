
//Handles the loading of 3D models with the help of Assimp, all vertex and index data is stored in meshes in an std::vector

#pragma once

#include <fstream>

#include "mesh.h"

#include <DirectXCollision.h>

using namespace DirectX;

namespace hrzn::gfx
{
	class Model
	{
	public:
		Model();

		bool initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext);

		template <class T>
		void draw(const XMMATRIX& modelMatrix, const XMMATRIX& viewProjectionMatrix, ConstantBuffer<T>* perObjectCB, bool bindTextures = true) const;

		void drawRaw(bool bindTextures = true);

		const std::vector<Mesh>&     getMeshes() const;

		const std::string&           getPath() const;
		const std::vector<XMFLOAT3>& getVertices() const;

		float              getHitRadius() const;
		const BoundingBox& getBoundingBox() const;
		bool               rayInersectAllFaces(XMVECTOR rayOrigin, XMVECTOR rayDirection, float* rayDistance) const;

	private:
		bool loadModel(const std::string& filePath);
		void processNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& transformMatrix);
		void loadModelMetaData(const std::string& filePath);

		int                   getTextureIndex(aiString* pString) const;
		TextureStorageType    determineTextureStorageType(const aiScene* pScene, aiMaterial* pMaterial, unsigned int index, aiTextureType textureType) const;
		std::vector<Texture*> loadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);

	private:
		std::vector<Mesh>     m_meshes;
		std::vector<XMFLOAT3> m_vertices;
		std::vector<DWORD>    m_indices;
		int                   m_currentNumVerts;

		ID3D11Device*         m_device;
		ID3D11DeviceContext*  m_deviceContext;

		std::string           m_filePath;
		std::string           m_directory;

		float                 m_modelHitRadius;
		BoundingBox           m_boundingBox;
	};

}

namespace hrzn::gfx
{
	template<class T>
	inline void Model::draw(const XMMATRIX& modelMatrix, const XMMATRIX& viewProjectionMatrix, ConstantBuffer<T>* perObjectCB, bool bindTextures) const
	{
		m_deviceContext->VSSetConstantBuffers(12, 1, perObjectCB->getAddressOf());

		for (int i = 0; i < m_meshes.size(); i++)
		{
			perObjectCB->m_data.m_modelMatrix = m_meshes[i].getTransformMatrix() * modelMatrix;
			//vertexShaderCB->data.modelMatrix = XMMatrixTranspose(vertexShaderCB->data.modelMatrix);

			perObjectCB->mapToGPU();

			m_meshes[i].draw(bindTextures);
		}
	}
}
