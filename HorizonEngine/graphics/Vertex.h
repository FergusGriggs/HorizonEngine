//Vertex.h
//Stores information about a single vertex of a mesh

#pragma once
#include <DirectXMath.h>

struct Vertex
{
	Vertex() : pos(0, 0, 0), normal(0, 0, 0), texCoord(0, 0) {}
	Vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v) : pos(x, y, z), normal(nx, ny, nz), texCoord(u, v) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 tangent;
	DirectX::XMFLOAT3 bitangent;
	DirectX::XMFLOAT2 texCoord;
};

