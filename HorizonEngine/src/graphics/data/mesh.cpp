#include "mesh.h"

#include "../graphics_handler.h"

namespace hrzn::gfx
{
	void Mesh::setMaterial(Material* material)
	{
		m_material = material;
	}

	void Mesh::draw(bool useGBuffer, bool bindPSData) const
    {
		if (m_hidden)
		{
			return;
		}

		ID3D11DeviceContext* deviceContext = GraphicsHandler::it().getDeviceContext();

		// If we have a material, bind it
		if (m_material != nullptr)
		{
			m_material->bind(useGBuffer, bindPSData);
		}

		UINT offset = 0;

		deviceContext->IASetVertexBuffers(0, 1, m_vertexBuffer.getAddressOfBuffer(), m_vertexBuffer.getStridePtr(), &offset);
		deviceContext->IASetIndexBuffer(m_indexBuffer.getBuffer(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
		deviceContext->DrawIndexed(m_indexBuffer.getIndexCount(), 0, 0);
    }

	const std::string& Mesh::getName() const
	{
		return m_name;
	}

	bool* Mesh::getHiddenBoolPtr()
	{
		return &m_hidden;
	}

	Material* Mesh::getMaterial()
	{
		return m_material;
	}

	const DirectX::XMMATRIX& Mesh::getTransformMatrix() const
	{
		return m_transformMatrix;
	}
}