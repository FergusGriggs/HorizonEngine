//Model.cpp
//Function implementations for the Model class

#include "Model.h"

bool Model::Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	this->device = device;
	this->deviceContext = deviceContext;

	try
		{
		if (!this->LoadModel(filePath)) {
			return false;
		}
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	
	return true;
}

void Model::Draw(const XMMATRIX& modelMatrix, const XMMATRIX& viewProjectionMatrix, ConstantBuffer<CB_VS_vertexShader>* cb_vs_vertexShader)
{
	this->deviceContext->VSSetConstantBuffers(0, 1, cb_vs_vertexShader->GetAddressOf());

	for (int i = 0; i < meshes.size(); i++) {
		cb_vs_vertexShader->data.modelViewProjectionMatrix = this->meshes[i].GetTransformMatrix() * modelMatrix * viewProjectionMatrix;
		//this->cb_vs_vertexShader->data.modelViewProjectionMatrix = XMMatrixTranspose(this->cb_vs_vertexShader->data.modelViewProjectionMatrix);

		cb_vs_vertexShader->data.modelMatrix = this->meshes[i].GetTransformMatrix() * modelMatrix;
		//this->cb_vs_vertexShader->data.modelMatrix = XMMatrixTranspose(this->cb_vs_vertexShader->data.modelMatrix);

		cb_vs_vertexShader->MapToGPU();

		meshes[i].Draw();
	}
}

float Model::GetHitRadius()
{
	return modelHitRadius;
}

bool Model::LoadModel(const std::string& filePath)
{
	this->directory = StringHelper::GetDirectoryFromPath(filePath);

	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr) {
		return false;
	}

	this->ProcessNode(pScene->mRootNode, pScene, XMMatrixIdentity());// * XMMatrixScaling(0.025f, 0.025f, 0.025f)

	this->LoadModelMetaData(filePath.substr(0, filePath.length() - 4) + "_meta.txt");//remove model file extension, add '_meta.txt'

	return true;
}

void Model::LoadModelMetaData(const std::string& filePath) {
	std::fstream metaDataFile(filePath);
	if (metaDataFile) {
		metaDataFile >> this->modelHitRadius;
		metaDataFile.close();
	}
}

void Model::ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix)
{
	XMMATRIX nodeTransformationMatrix = XMMatrixTranspose(XMMATRIX(&node->mTransformation.a1))* parentTransformMatrix;

	for (UINT i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(this->ProcessMesh(mesh, scene, nodeTransformationMatrix));
	}

	for (UINT i = 0; i < node->mNumChildren; i++) {
		this->ProcessNode(node->mChildren[i], scene, nodeTransformationMatrix);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& transformMatrix)
{
	std::vector<Vertex> vertices;
	std::vector<DWORD> indices;

	for (UINT i = 0; i < mesh->mNumVertices; i++) {

		Vertex vertex;

		vertex.pos.x = mesh->mVertices[i].x;
		vertex.pos.y = mesh->mVertices[i].y;
		vertex.pos.z = mesh->mVertices[i].z;

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0]) {
			vertex.tangent.x = mesh->mTangents[i].x;
			vertex.tangent.y = mesh->mTangents[i].y;
			vertex.tangent.z = mesh->mTangents[i].z;

			vertex.bitangent.x = mesh->mBitangents[i].x;
			vertex.bitangent.y = mesh->mBitangents[i].y;
			vertex.bitangent.z = mesh->mBitangents[i].z;

		
			vertex.texCoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back(vertex);
	}


	for (UINT i = 0; i < mesh->mNumFaces; i++) {

		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	std::vector<Texture> textures;
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	std::vector<Texture> diffuseTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, scene);
	textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());

	std::vector<Texture> specularTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_SPECULAR, scene);
	textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());

	std::vector<Texture> normalTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_HEIGHT, scene);
	textures.insert(textures.end(), normalTextures.begin(), normalTextures.end());

	std::vector<Texture> depthTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_DISPLACEMENT, scene);
	textures.insert(textures.end(), depthTextures.begin(), depthTextures.end());

	return Mesh(this->device, this->deviceContext, vertices, indices, textures, transformMatrix);
}

int Model::GetTextureIndex(aiString* pString)
{
	assert(pString->length >= 2);
	return atoi((&pString->C_Str()[1]));
}

TextureStorageType Model::DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMaterial, unsigned int index, aiTextureType textureType)
{
	if (pMaterial->GetTextureCount(textureType) == 0) {
		return TextureStorageType::NONE;
	}

	aiString path;
	pMaterial->GetTexture(textureType, index, &path);
	std::string texturePath = path.C_Str();

	if (texturePath[0] == '*') {
		if (pScene->mTextures[0]->mHeight == 0) {
			return TextureStorageType::EMBEDDED_INDEX_COMPRESSED;
		}
		else {
			//assert("COMPRESSED TEXTURES ARE NOT SUPPORTED." && );
			return TextureStorageType::EMBEDDED_INDEX_NON_COMPRESSED;
		}
	}
	if (auto pTex = pScene->GetEmbeddedTexture(texturePath.c_str())) {
		if (pTex->mHeight == 0) {
			return TextureStorageType::EMBEDDED_COMPRESSED;
		}
		else {
			//assert("COMPRESSED TEXTURES ARE NOT SUPPORTED." && );
			return TextureStorageType::EMBEDDED_NON_COMPRESSED;
		}
	}
	if (texturePath.find('.') != std::string::npos) {
		return TextureStorageType::DISK;
	}

	return TextureStorageType::NONE;
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene)
{
	std::vector<Texture> materialTextures;
	TextureStorageType storageType = TextureStorageType::INVALID;
	unsigned int textureCount = pMaterial->GetTextureCount(textureType);

	if (textureCount == 0) {
		storageType = TextureStorageType::NONE;
		aiColor3D aiColour(0.0f, 0.0f, 0.0f);

		switch (textureType) {
		case aiTextureType_DIFFUSE:
			pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColour);

			if (aiColour.IsBlack()) {
				materialTextures.push_back(Texture(this->device, Colours::UnloadedTextureColour, textureType));
				return materialTextures;
			}

			materialTextures.push_back(Texture(this->device, Colour(aiColour.r * 255, aiColour.g * 255, aiColour.b * 255), textureType));
			return materialTextures;

		case aiTextureType_SPECULAR:
			pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, aiColour);

			if (aiColour.IsBlack()) {
				materialTextures.push_back(Texture(this->device, Colours::UnloadedSpecularTextureColour, textureType));
				return materialTextures;
			}

			materialTextures.push_back(Texture(this->device, Colour(aiColour.r * 255, aiColour.g * 255, aiColour.b * 255), textureType));
			return materialTextures;

		case aiTextureType_HEIGHT:
			materialTextures.push_back(Texture(this->device, Colours::UnloadedNormalTextureColour, textureType));
			return materialTextures;
		
		case aiTextureType_DISPLACEMENT:
			materialTextures.push_back(Texture(this->device, Colours::UnloadedDepthTextureColour, textureType));
			return materialTextures;
		}
	}
	else {
		for (UINT i = 0; i < textureCount; i++) {
			aiString path;
			pMaterial->GetTexture(textureType, i, &path);
			TextureStorageType storageType = DetermineTextureStorageType(pScene, pMaterial, i, textureType);

			switch (storageType) {
			case TextureStorageType::DISK:
			{
				std::string fileName = this->directory + "/" + path.C_Str();
				Texture diskTexture = Texture(this->device, fileName, textureType);
				materialTextures.push_back(diskTexture);
				break;
			}
			case TextureStorageType::EMBEDDED_COMPRESSED:
			{
				const aiTexture* pTexture = pScene->GetEmbeddedTexture(path.C_Str());
				Texture embeddedTexture = Texture(this->device, reinterpret_cast<uint8_t*>(pTexture->pcData), pTexture->mWidth, textureType);
				materialTextures.push_back(embeddedTexture);
				break;
			}
			case TextureStorageType::EMBEDDED_INDEX_COMPRESSED:
				int index = GetTextureIndex(&path);
				Texture embeddedIndexedTexture = Texture(this->device, reinterpret_cast<uint8_t*>(pScene->mTextures[index]->pcData), pScene->mTextures[index]->mWidth, textureType);
				materialTextures.push_back(embeddedIndexedTexture);
				break;
			}
		}
		return materialTextures;
	}

	if (materialTextures.size() == 0) {
		materialTextures.push_back(Texture(this->device, Colours::UnhandledTextureColour, textureType));
		return materialTextures;
	}
}

