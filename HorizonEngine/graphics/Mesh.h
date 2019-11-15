//Mesh.h
//Stores all information required to draw a single mesh of a model

#pragma once

#include "Vertex.h"
#include "buffers/VertexBuffer.h"
#include "buffers/IndexBuffer.h"
#include "buffers/ConstantBuffer.h"
#include "Texture.h"

#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

class Mesh
{
public:
	Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<Vertex>& vertices, std::vector<DWORD>& indices, std::vector<Texture> & textures, const DirectX::XMMATRIX& transformMatrix);
	Mesh(const Mesh& mesh);
	void Draw();
	const DirectX::XMMATRIX& GetTransformMatrix();

private:
	VertexBuffer<Vertex> vertexBuffer;
	IndexBuffer indexBuffer;
	ID3D11DeviceContext* deviceContext;
	std::vector<Texture> textures;
	DirectX::XMMATRIX transformMatrix;
};

