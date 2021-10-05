//Model.h
//Handles the loading of 3D models with the help of Assimp, all vertex and index data is stored in meshes in an std::vector

#pragma once

#include "Mesh.h"
#include <fstream>

#include <DirectXCollision.h>

class ResourceManager;

using namespace DirectX;

class Model
{
public:
	bool Initialize(const std::string & filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ResourceManager* resourceManager);
	void Draw(const XMMATRIX& modelMatrix, const XMMATRIX& viewProjectionMatrix, ConstantBuffer<CB_VS_vertexShader>* cb_vs_vertexShader, bool bindTextures = true);
	float GetHitRadius();
	std::string GetPath();

	BoundingBox GetBoundingBox();
	std::vector<XMFLOAT3>* GetVertices();
	bool RayInersect(XMVECTOR rayOrigin, XMVECTOR rayDirection, float* rayDistance);

private:
	bool LoadModel(const std::string& filePath);
	void ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& transformMatrix);
	void LoadModelMetaData(const std::string& filePath);

	int GetTextureIndex(aiString * pString);
	TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMaterial, unsigned int index, aiTextureType textureType);
	std::vector<Texture*> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);

	std::vector<Mesh> meshes;

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;
	ResourceManager* resourceManager = nullptr;

	std::string filePath = "";
	std::string directory = "";
	float modelHitRadius = 0.5f;

	BoundingBox boundingBox;

	std::vector<XMFLOAT3> vertices;
	int currentNumVerts = 0;
	std::vector<DWORD> indices;
};

#include "utility/ResourceManager.h";