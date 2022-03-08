//Vertex.h
//Stores information about a single vertex of a mesh

#pragma once

#include <DirectXMath.h>

namespace hrzn::gfx
{
	struct Vertex
	{
		Vertex() :
			m_pos(0.0f, 0.0f, 0.0f),
			m_normal(0.0f, 0.0f, 0.0f),
			m_tangent(0.0f, 0.0f, 0.0f),
			m_bitangent(0.0f, 0.0f, 0.0f),
			m_texCoord(0.0f, 0.0f)
		{
		}

		Vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v) :
			m_pos(x, y, z),
			m_normal(nx, ny, nz),
			m_tangent(0.0f, 0.0f, 0.0f),
			m_bitangent(0.0f, 0.0f, 0.0f),
			m_texCoord(u, v)
		{
		}

		DirectX::XMFLOAT3 m_pos;
		DirectX::XMFLOAT3 m_normal;
		DirectX::XMFLOAT3 m_tangent;
		DirectX::XMFLOAT3 m_bitangent;
		DirectX::XMFLOAT2 m_texCoord;
	};
}
