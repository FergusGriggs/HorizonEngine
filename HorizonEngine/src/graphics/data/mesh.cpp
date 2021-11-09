//Mesh.cpp
//Function implementations for the Mesh class

#include "Mesh.h"

namespace hrzn::gfx
{
	Mesh::Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<Vertex>& vertices, std::vector<DWORD>& indices, std::vector<Texture*>& textures, const DirectX::XMMATRIX& transformMatrix) :
		m_vertexBuffer(),
		m_indexBuffer(),

		m_textures(textures),

		m_transformMatrix(transformMatrix),

		m_deviceContext(deviceContext)
	{
		HRESULT hr = m_vertexBuffer.Initialize(device, vertices.data(), vertices.size());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh.");

		hr = m_indexBuffer.initialize(device, indices.data(), indices.size());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh.");
	}

	Mesh::Mesh(const Mesh& mesh) :
		m_deviceContext(mesh.m_deviceContext),
		m_indexBuffer(mesh.m_indexBuffer),
		m_vertexBuffer(mesh.m_vertexBuffer),
		m_textures(mesh.m_textures),
		m_transformMatrix(mesh.m_transformMatrix)
	{
	}

	void Mesh::draw(bool bindTextures) const
	{
		if (bindTextures)
		{
			for (int i = 0; i < m_textures.size(); i++)
			{
				switch (m_textures[i]->getType())
				{
				case aiTextureType::aiTextureType_DIFFUSE:
					m_deviceContext->PSSetShaderResources(0, 1, m_textures[i]->getTextureResourceViewAddress());
					break;
				case aiTextureType::aiTextureType_SPECULAR:
					m_deviceContext->PSSetShaderResources(1, 1, m_textures[i]->getTextureResourceViewAddress());
					break;
				case aiTextureType::aiTextureType_NORMALS:
					m_deviceContext->PSSetShaderResources(2, 1, m_textures[i]->getTextureResourceViewAddress());
					break;
				case aiTextureType::aiTextureType_DISPLACEMENT:
					m_deviceContext->PSSetShaderResources(3, 1, m_textures[i]->getTextureResourceViewAddress());
					break;
				}
			}
		}

		UINT offset = 0;

		m_deviceContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), m_vertexBuffer.StridePtr(), &offset);
		m_deviceContext->IASetIndexBuffer(m_indexBuffer.get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
		m_deviceContext->DrawIndexed(m_indexBuffer.indexCount(), 0, 0);
	}

	const DirectX::XMMATRIX& Mesh::getTransformMatrix() const
	{
		return m_transformMatrix;
	}
}
