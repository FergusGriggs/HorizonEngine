#pragma once

#include "Mesh.h"

using namespace DirectX;

class Model
{
public:
	bool Initialize(const std::string & filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexShader>& cb_vs_vertexShader);
	void Draw(const XMMATRIX& modelMatrix, const XMMATRIX& viewProjectionMatrix);

private:
	bool LoadModel(const std::string& filePath);
	void ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& transformMatrix);

	int GetTextureIndex(aiString * pString);
	TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMaterial, unsigned int index, aiTextureType textureType);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);

	std::vector<Mesh> meshes;

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;
	ConstantBuffer<CB_VS_vertexShader>* cb_vs_vertexShader = nullptr;

	std::string directory = "";
};

