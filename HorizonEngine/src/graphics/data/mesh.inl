#pragma once

#include <DirectXMath.h>

#include "mesh.h"

namespace hrzn::gfx
{
	template<typename VertexType>
	inline Mesh::Mesh(std::vector<VertexType>& vertices, std::vector<DWORD>& indices) :
		m_name("null"),

		m_material(nullptr),

		m_transformMatrix(DirectX::XMMatrixIdentity()),

		m_hidden(false)
	{
		createBuffers<VertexType>(vertices, indices);
	}

	template<typename VertexType>
	inline Mesh::Mesh(std::vector<VertexType>& vertices, std::vector<DWORD>& indices, Material* material) :
		m_name("null"),

		m_material(material),

		m_transformMatrix(DirectX::XMMatrixIdentity()),

		m_hidden(false)
	{
		createBuffers<VertexType>(vertices, indices);
	}

	template<typename VertexType>
	inline Mesh::Mesh(const std::string& name, std::vector<VertexType>& vertices, std::vector<DWORD>& indices, Material* material) : 
		m_name(name),

		m_material(material),

		m_transformMatrix(DirectX::XMMatrixIdentity()),

		m_hidden(false)
	{
		createBuffers<VertexType>(vertices, indices);
	}

	template<typename VertexType>
	inline Mesh::Mesh(const std::string& name, std::vector<VertexType>& vertices, std::vector<DWORD>& indices, const DirectX::XMMATRIX& transformMatrix) :
		m_name(name),

		m_material(nullptr),

		m_transformMatrix(transformMatrix),

		m_hidden(false)
	{
		createBuffers<VertexType>(vertices, indices);
	}

	template<typename VertexType>
	inline Mesh::Mesh(const std::string& name, std::vector<VertexType>& vertices, std::vector<DWORD>& indices, Material* material, const DirectX::XMMATRIX& transformMatrix) :
		m_name(name),

		m_material(material),

		m_transformMatrix(transformMatrix),

		m_hidden(false)
	{
		createBuffers<VertexType>(vertices, indices);
	}

	inline Mesh::Mesh(const Mesh& mesh) :
		m_name(mesh.m_name),
		m_indexBuffer(mesh.m_indexBuffer),
		m_vertexBuffer(mesh.m_vertexBuffer),
		m_material(mesh.m_material),
		m_transformMatrix(mesh.m_transformMatrix),
		m_hidden(mesh.m_hidden)
	{
	}

	template<typename VertexType>
	inline void Mesh::createBuffers(std::vector<VertexType>& vertices, std::vector<DWORD>& indices)
	{
		HRESULT hr = m_vertexBuffer.initialise(vertices.data(), sizeof(VertexType), static_cast<UINT>(vertices.size()));
		COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh");

		hr = m_indexBuffer.initialise(indices.data(), static_cast<UINT>(indices.size()));
		COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh");
	}
}
