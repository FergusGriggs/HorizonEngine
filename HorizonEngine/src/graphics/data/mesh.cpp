//Mesh.cpp
//Function implementations for the Mesh class

#include "Mesh.h"
#include "../graphics_handler.h"

namespace hrzn::gfx
{
	Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<DWORD>& indices, Material* material, const DirectX::XMMATRIX& transformMatrix) :
		m_vertexBuffer(),
		m_indexBuffer(),

		m_material(material),

		m_transformMatrix(transformMatrix)
	{
		HRESULT hr = m_vertexBuffer.Initialize(GraphicsHandler::it().getDevice(), vertices.data(), static_cast<UINT>(vertices.size()));
		COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh.");

		hr = m_indexBuffer.initialize(GraphicsHandler::it().getDevice(), indices.data(), static_cast<UINT>(indices.size()));
		COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh.");
	}

	Mesh::Mesh(const Mesh& mesh) :
		m_indexBuffer(mesh.m_indexBuffer),
		m_vertexBuffer(mesh.m_vertexBuffer),
		m_material(mesh.m_material),
		m_transformMatrix(mesh.m_transformMatrix)
	{
	}

	void Mesh::draw(bool useGBuffer, bool bindPSData) const
	{
		ID3D11DeviceContext* deviceContext = GraphicsHandler::it().getDeviceContext();

		m_material->bind(useGBuffer, bindPSData);

		UINT offset = 0;

		deviceContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), m_vertexBuffer.StridePtr(), &offset);
		deviceContext->IASetIndexBuffer(m_indexBuffer.get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
		deviceContext->DrawIndexed(m_indexBuffer.indexCount(), 0, 0);
	}

	const DirectX::XMMATRIX& Mesh::getTransformMatrix() const
	{
		return m_transformMatrix;
	}
}
