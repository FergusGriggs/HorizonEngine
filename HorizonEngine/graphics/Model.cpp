//Model.cpp
//Function implementations for the Model class

#include "Model.h"

bool Model::Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ResourceManager* resourceManager)
{
	this->filePath = filePath;
	this->device = device;
	this->deviceContext = deviceContext;
	this->resourceManager = resourceManager;

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

void Model::Draw(const XMMATRIX& modelMatrix, const XMMATRIX& viewProjectionMatrix, ConstantBuffer<CB_VS_vertexShader>* cb_vs_vertexShader, bool bindTextures)
{
	this->deviceContext->VSSetConstantBuffers(0, 1, cb_vs_vertexShader->GetAddressOf());

	for (int i = 0; i < meshes.size(); i++)
	{
		cb_vs_vertexShader->data.modelViewProjectionMatrix = this->meshes[i].GetTransformMatrix() * modelMatrix * viewProjectionMatrix;
		//this->cb_vs_vertexShader->data.modelViewProjectionMatrix = XMMatrixTranspose(this->cb_vs_vertexShader->data.modelViewProjectionMatrix);

		cb_vs_vertexShader->data.modelMatrix = this->meshes[i].GetTransformMatrix() * modelMatrix;
		//this->cb_vs_vertexShader->data.modelMatrix = XMMatrixTranspose(this->cb_vs_vertexShader->data.modelMatrix);

		cb_vs_vertexShader->MapToGPU();

		meshes[i].Draw(bindTextures);
	}
}

float Model::GetHitRadius()
{
	return modelHitRadius;
}

std::string Model::GetPath()
{
	return this->filePath;
}

BoundingBox Model::GetBoundingBox()
{
	return this->boundingBox;
}

std::vector<XMFLOAT3>* Model::GetVertices()
{
	return &this->vertices;
}

bool Model::RayInersect(XMVECTOR rayOrigin, XMVECTOR rayDirection, float* rayDistance)
{
	for (int i = 0; i < this->indices.size(); i += 3)
	{
		// Load vertex positions
		XMVECTOR vertex1 = XMLoadFloat3(&this->vertices[this->indices[i]]);
		XMVECTOR vertex2 = XMLoadFloat3(&this->vertices[this->indices[i + 1]]);
		XMVECTOR vertex3 = XMLoadFloat3(&this->vertices[this->indices[i + 2]]);

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

bool Model::LoadModel(const std::string& filePath)
{
	this->directory = StringHelper::GetDirectoryFromPath(filePath);

	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr) {
		return false;
	}

	this->ProcessNode(pScene->mRootNode, pScene, XMMatrixIdentity());// * XMMatrixScaling(0.025f, 0.025f, 0.025f)

	BoundingBox::CreateFromPoints(this->boundingBox, vertices.size(), &(vertices.at(0)), sizeof(XMFLOAT3));

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
	std::vector<Vertex> meshVertices;
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

		meshVertices.push_back(vertex);
		this->vertices.push_back(vertex.pos);
	}


	for (UINT i = 0; i < mesh->mNumFaces; i++) {

		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
			this->indices.push_back(this->currentNumVerts + face.mIndices[j]);
		}
	}

	this->currentNumVerts = this->vertices.size();

	std::vector<Texture*> textures;
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	std::vector<Texture*> diffuseTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, scene);
	textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());

	std::vector<Texture*> specularTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_SPECULAR, scene);
	textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());

	std::vector<Texture*> normalTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_HEIGHT, scene);
	textures.insert(textures.end(), normalTextures.begin(), normalTextures.end());

	std::vector<Texture*> depthTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_DISPLACEMENT, scene);
	textures.insert(textures.end(), depthTextures.begin(), depthTextures.end());

	return Mesh(this->device, this->deviceContext, meshVertices, indices, textures, transformMatrix);
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

std::vector<Texture*> Model::LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene)
{
	std::vector<Texture*> materialTextures;
	TextureStorageType storageType = TextureStorageType::INVALID;
	unsigned int textureCount = pMaterial->GetTextureCount(textureType);

	if (textureCount == 0) {
		storageType = TextureStorageType::NONE;
		aiColor3D aiColour(0.0f, 0.0f, 0.0f);

		switch (textureType) {
		case aiTextureType_DIFFUSE:
		{
			pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColour);

			if (aiColour.IsBlack()) {
				materialTextures.push_back(resourceManager->GetColourTexturePtr("UNLOADED_DIFFUSE", Colours::UnloadedTextureColour, textureType));
				return materialTextures;
			}
			std::string key = std::to_string(static_cast<int>(aiColour.r * 255)) + "|" + std::to_string(static_cast<int>(aiColour.g * 255)) + "|" + std::to_string(static_cast<int>(aiColour.b * 255));
			materialTextures.push_back(resourceManager->GetColourTexturePtr(key, Colour(aiColour.r * 255, aiColour.g * 255, aiColour.b * 255), textureType));
			return materialTextures;
		}
		case aiTextureType_SPECULAR:
			materialTextures.push_back(resourceManager->GetColourTexturePtr("UNLOADED_SPECULAR", Colours::UnloadedSpecularTextureColour, textureType));
			return materialTextures;

			return materialTextures;

		case aiTextureType_HEIGHT:
			materialTextures.push_back(resourceManager->GetColourTexturePtr("UNLOADED_NORMAL", Colours::UnloadedNormalTextureColour, textureType));
			return materialTextures;
		
		case aiTextureType_DISPLACEMENT:
			materialTextures.push_back(resourceManager->GetColourTexturePtr("UNLOADED_DEPTH", Colours::UnloadedDepthTextureColour, textureType));
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
				materialTextures.push_back(resourceManager->GetTexturePtr(fileName, textureType));
				break;
			}
			case TextureStorageType::EMBEDDED_COMPRESSED:
			{
				const aiTexture* pTexture = pScene->GetEmbeddedTexture(path.C_Str());
				std::string key = this->directory + "/" + path.C_Str();
				materialTextures.push_back(resourceManager->GetTexturePtr(key, reinterpret_cast<uint8_t*>(pTexture->pcData), pTexture->mWidth, textureType));
				break;
			}
			case TextureStorageType::EMBEDDED_INDEX_COMPRESSED:
			{
				int index = GetTextureIndex(&path);
				std::string key = this->directory + "/" + path.C_Str();
				materialTextures.push_back(resourceManager->GetTexturePtr(key, reinterpret_cast<uint8_t*>(pScene->mTextures[index]->pcData), pScene->mTextures[index]->mWidth, textureType));
				break;
			}
			}
		}
		return materialTextures;
	}

	if (materialTextures.size() == 0) {
		materialTextures.push_back(resourceManager->GetColourTexturePtr("UNHANDLED", Colours::UnhandledTextureColour, textureType));
		return materialTextures;
	}
}

