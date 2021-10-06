
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

		bool        initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
		void        draw(const XMMATRIX& modelMatrix, const XMMATRIX& viewProjectionMatrix, ConstantBuffer<CB_VS_vertexShader>* cb_vs_vertexShader, bool bindTextures = true);

		std::string            getPath();
		std::vector<XMFLOAT3>* getVertices();

		float       getHitRadius();
		BoundingBox getBoundingBox();
		bool        rayInersect(XMVECTOR rayOrigin, XMVECTOR rayDirection, float* rayDistance);

	private:
		bool loadModel(const std::string& filePath);
		void processNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& transformMatrix);
		void loadModelMetaData(const std::string& filePath);

		int                   getTextureIndex(aiString* pString);
		TextureStorageType    determineTextureStorageType(const aiScene* pScene, aiMaterial* pMaterial, unsigned int index, aiTextureType textureType);
		std::vector<Texture*> loadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);

	private:
		std::vector<Mesh>     m_meshes;
		std::vector<XMFLOAT3> m_vertices;
		std::vector<DWORD>    m_indices;
		int                   m_currentNumVerts;

		ID3D11Device* m_device;
		ID3D11DeviceContext* m_deviceContext;

		std::string           m_filePath;
		std::string           m_directory;

		float                 m_modelHitRadius;
		BoundingBox           m_boundingBox;
	};
}
