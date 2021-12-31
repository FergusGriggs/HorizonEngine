//Mesh.h
//Stores all information required to draw a single mesh of a model

#pragma once

#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "vertex.h"
#include "texture.h"
#include "material.h"

#include "../buffers/vertex_buffer.h"
#include "../buffers/index_buffer.h"
#include "../buffers/constant_buffer.h"

namespace hrzn::gfx
{
	class Mesh
	{
	public:
		Mesh(std::vector<Vertex>& vertices, std::vector<DWORD>& indices, Material* material, const DirectX::XMMATRIX& transformMatrix);
		Mesh(const Mesh& mesh);

		void draw(bool useGBuffer, bool bindPSData = true) const;
		const DirectX::XMMATRIX& getTransformMatrix() const;

	private:
		VertexBuffer<Vertex>  m_vertexBuffer;
		IndexBuffer           m_indexBuffer;

		Material*             m_material;

		DirectX::XMMATRIX     m_transformMatrix;
	};
}
