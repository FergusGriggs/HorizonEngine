#include "Mesh.h"

Mesh::Mesh(ID3D11Device* device, ID3D11DeviceContext * deviceContext, std::vector<Vertex>& vertices, std::vector<DWORD>& indices, std::vector<Texture>& textures, const DirectX::XMMATRIX& transformMatrix)
{
	this->deviceContext = deviceContext;
	this->textures = textures;
	this->transformMatrix = transformMatrix;

	HRESULT hr = this->vertexBuffer.Initialize(device, vertices.data(), vertices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh.");

	hr = this->indexBuffer.Initialize(device, indices.data(), indices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh.");
}

Mesh::Mesh(const Mesh& mesh)
{
	this->deviceContext = mesh.deviceContext;
	this->indexBuffer = mesh.indexBuffer;
	this->vertexBuffer = mesh.vertexBuffer;
	this->textures = mesh.textures;
	this->transformMatrix = mesh.transformMatrix;
}

void Mesh::Draw()
{
	UINT offset = 0;

	for (int i = 0; i < textures.size(); i++) {
		switch (textures[i].GetType())
		{
		case aiTextureType::aiTextureType_DIFFUSE:
			this->deviceContext->PSSetShaderResources(0, 1, textures[i].GetTextureResourceViewAddress());
			break;
		case aiTextureType::aiTextureType_SPECULAR:
			this->deviceContext->PSSetShaderResources(1, 1, textures[i].GetTextureResourceViewAddress());
			break;
		case aiTextureType::aiTextureType_HEIGHT:
			this->deviceContext->PSSetShaderResources(2, 1, textures[i].GetTextureResourceViewAddress());
			break;
		case aiTextureType::aiTextureType_DISPLACEMENT:
			this->deviceContext->PSSetShaderResources(3, 1, textures[i].GetTextureResourceViewAddress());
			break;
		}
	}

	this->deviceContext->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), this->vertexBuffer.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(this->indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	this->deviceContext->DrawIndexed(this->indexBuffer.IndexCount(), 0, 0);
}

const DirectX::XMMATRIX& Mesh::GetTransformMatrix()
{
	return this->transformMatrix;
}
