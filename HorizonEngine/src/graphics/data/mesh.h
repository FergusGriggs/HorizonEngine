//Mesh.h
//Stores all information required to draw a single mesh of a model

#pragma once

#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "vertex.h"
#include "texture.h"

#include "../buffers/vertex_buffer.h"
#include "../buffers/index_buffer.h"
#include "../buffers/constant_buffer.h"

namespace hrzn::gfx
{
	class Mesh
	{
	public:
		Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<Vertex>& vertices, std::vector<DWORD>& indices, std::vector<Texture*>& textures, const DirectX::XMMATRIX& transformMatrix);
		Mesh(const Mesh& mesh);

		void draw(bool bindTextures = true) const;
		const DirectX::XMMATRIX& getTransformMatrix() const;

	private:
		VertexBuffer<Vertex>  m_vertexBuffer;
		IndexBuffer           m_indexBuffer;

		std::vector<Texture*> m_textures;

		DirectX::XMMATRIX     m_transformMatrix;

		ID3D11DeviceContext*  m_deviceContext;
	};
}
