#pragma once

#include <vector>
#include <string>

#include <DirectXMath.h>

#include <Assimp/Importer.hpp>
#include <Assimp/postprocess.h>
#include <Assimp/scene.h>

#include "../buffers/vertex_buffer.h"
#include "../buffers/index_buffer.h"

#include "material.h"

// Forward declarations
namespace hrzn::gfx
{
	class Texture;
	class Material;
}

namespace hrzn::gfx
{
	class Mesh
	{
	public:
		template<typename VertexType>
		Mesh(std::vector<VertexType>& vertices, std::vector<DWORD>& indices);
		template<typename VertexType>
		Mesh(std::vector<VertexType>& vertices, std::vector<DWORD>& indices, Material* material);
		template<typename VertexType>
		Mesh(const std::string& name, std::vector<VertexType>& vertices, std::vector<DWORD>& indices, Material* material);
		template<typename VertexType>
		Mesh(const std::string& name, std::vector<VertexType>& vertices, std::vector<DWORD>& indices, const DirectX::XMMATRIX& transformMatrix);
		template<typename VertexType>
		Mesh(const std::string& name, std::vector<VertexType>& vertices, std::vector<DWORD>& indices, Material* material, const DirectX::XMMATRIX& transformMatrix);

		Mesh(const Mesh& mesh);

		template<typename VertexType>
		void createBuffers(std::vector<VertexType>& vertices, std::vector<DWORD>& indices);

		void setMaterial(Material* material);

		void                     draw(bool useGBuffer, bool bindPSData = true) const;

		const std::string&       getName() const;
		bool*                    getHiddenBoolPtr();

		Material*                getMaterial();

		const DirectX::XMMATRIX& getTransformMatrix() const;

		
	private:
		std::string  m_name;

		VertexBuffer m_vertexBuffer;
		IndexBuffer  m_indexBuffer;

		Material* m_material;

		DirectX::XMMATRIX m_transformMatrix;

		bool m_hidden;
	};
}

#include "mesh.inl"
