#include "vertex_types.h"

namespace hrzn::gfx
{
	UnlitVertex::UnlitVertex() :
		m_pos(0.0f, 0.0f, 0.0f),
		m_texCoord(0.0f, 0.0f)
	{
	}

	UnlitVertex::UnlitVertex(const maths::Vec3f& pos, const maths::Vec2f& texCoord) :
		m_pos(pos),
		m_texCoord(texCoord)
	{
	}

	void UnlitVertex::fillUsingAssimpMesh(aiMesh* assimpMesh, int vertexIndex)
	{
		m_pos.x = (float)assimpMesh->mVertices[vertexIndex].x;
		m_pos.y = (float)assimpMesh->mVertices[vertexIndex].y;
		m_pos.z = (float)assimpMesh->mVertices[vertexIndex].z;

		if (assimpMesh->mTextureCoords[0])
		{
			m_texCoord.x = (float)assimpMesh->mTextureCoords[0][vertexIndex].x;
			m_texCoord.y = (float)assimpMesh->mTextureCoords[0][vertexIndex].y;
		}
	}

	UnlitColourVertex::UnlitColourVertex() :
		UnlitVertex(),
		m_colour(0.0f, 0.0f, 0.0f, 0.0f)
	{
	}

	UnlitColourVertex::UnlitColourVertex(const maths::Vec3f& pos, const maths::Vec2f& texCoord, const maths::Vec4f& colour) :
		UnlitVertex(pos, texCoord),
		m_colour(colour)
	{
	}

	SimpleLitVertex::SimpleLitVertex() :
		m_pos(0.0f, 0.0f, 0.0f),
		m_normal(0.0f, 0.0f, 0.0f),
		m_texCoord(0.0f, 0.0f)
	{
	}

	SimpleLitVertex::SimpleLitVertex(const maths::Vec3f& pos, const maths::Vec3f& normal, const maths::Vec2f& texCoord) :
		m_pos(pos),
		m_normal(normal),
		m_texCoord(texCoord)
	{
	}

	void SimpleLitVertex::fillUsingAssimpMesh(aiMesh* assimpMesh, int vertexIndex)
	{
		m_pos.x = (float)assimpMesh->mVertices[vertexIndex].x;
		m_pos.y = (float)assimpMesh->mVertices[vertexIndex].y;
		m_pos.z = (float)assimpMesh->mVertices[vertexIndex].z;

		m_normal.x = (float)assimpMesh->mNormals[vertexIndex].x;
		m_normal.y = (float)assimpMesh->mNormals[vertexIndex].y;
		m_normal.z = (float)assimpMesh->mNormals[vertexIndex].z;

		if (assimpMesh->mTextureCoords[0])
		{
			m_texCoord.x = (float)assimpMesh->mTextureCoords[0][vertexIndex].x;
			m_texCoord.y = (float)assimpMesh->mTextureCoords[0][vertexIndex].y;
		}
	}

	FancyLitVertex::FancyLitVertex() :
		m_pos(0.0f, 0.0f, 0.0f),
		m_normal(0.0f, 0.0f, 0.0f),
		m_tangent(0.0f, 0.0f, 0.0f),
		m_bitangent(0.0f, 0.0f, 0.0f),
		m_texCoord(0.0f, 0.0f)
	{
	}

	FancyLitVertex::FancyLitVertex(const maths::Vec3f& pos, const maths::Vec3f& normal, const maths::Vec3f& tangent, const maths::Vec3f& bitangent, const maths::Vec2f& texCoord) :
		m_pos(pos),
		m_normal(normal),
		m_tangent(tangent),
		m_bitangent(bitangent),
		m_texCoord(texCoord)
	{
	}

	void FancyLitVertex::fillUsingAssimpMesh(aiMesh* assimpMesh, int vertexIndex)
	{
		m_pos.x = (float)assimpMesh->mVertices[vertexIndex].x;
		m_pos.y = (float)assimpMesh->mVertices[vertexIndex].y;
		m_pos.z = (float)assimpMesh->mVertices[vertexIndex].z;

		m_normal.x = (float)assimpMesh->mNormals[vertexIndex].x;
		m_normal.y = (float)assimpMesh->mNormals[vertexIndex].y;
		m_normal.z = (float)assimpMesh->mNormals[vertexIndex].z;

		if (assimpMesh->mTextureCoords[0])
		{
			m_tangent.x = (float)assimpMesh->mTangents[vertexIndex].x;
			m_tangent.y = (float)assimpMesh->mTangents[vertexIndex].y;
			m_tangent.z = (float)assimpMesh->mTangents[vertexIndex].z;

			m_bitangent.x = (float)assimpMesh->mBitangents[vertexIndex].x;
			m_bitangent.y = (float)assimpMesh->mBitangents[vertexIndex].y;
			m_bitangent.z = (float)assimpMesh->mBitangents[vertexIndex].z;

			m_texCoord.x = (float)assimpMesh->mTextureCoords[0][vertexIndex].x;
			m_texCoord.y = (float)assimpMesh->mTextureCoords[0][vertexIndex].y;
		}
	}

	SkinnedVertex::SkinnedVertex() :
		m_pos(0.0f, 0.0f, 0.0f),
		m_normal(0.0f, 0.0f, 0.0f),
		m_tangent(0.0f, 0.0f, 0.0f),
		m_bitangent(0.0f, 0.0f, 0.0f),
		m_texCoord(0.0f, 0.0f)
	{
	}

	SkinnedVertex::SkinnedVertex(const maths::Vec3f& pos, const maths::Vec3f& normal, const maths::Vec3f& tangent, const maths::Vec3f& bitangent, const maths::Vec2f& texCoord,
		const maths::Vec4f& boneWeights, const maths::Vec4<int>& boneIndices) :
		m_pos(pos),
		m_normal(normal),
		m_tangent(tangent),
		m_bitangent(bitangent),
		m_texCoord(texCoord),
		m_boneWeights(boneWeights),
		m_boneIndices(boneIndices)
	{
	}

	void SkinnedVertex::fillUsingAssimpMesh(aiMesh* assimpMesh, int vertexIndex)
	{
		m_pos.x = (float)assimpMesh->mVertices[vertexIndex].x;
		m_pos.y = (float)assimpMesh->mVertices[vertexIndex].y;
		m_pos.z = (float)assimpMesh->mVertices[vertexIndex].z;

		m_normal.x = (float)assimpMesh->mNormals[vertexIndex].x;
		m_normal.y = (float)assimpMesh->mNormals[vertexIndex].y;
		m_normal.z = (float)assimpMesh->mNormals[vertexIndex].z;

		if (assimpMesh->mTextureCoords[0])
		{
			m_tangent.x = (float)assimpMesh->mTangents[vertexIndex].x;
			m_tangent.y = (float)assimpMesh->mTangents[vertexIndex].y;
			m_tangent.z = (float)assimpMesh->mTangents[vertexIndex].z;

			m_bitangent.x = (float)assimpMesh->mBitangents[vertexIndex].x;
			m_bitangent.y = (float)assimpMesh->mBitangents[vertexIndex].y;
			m_bitangent.z = (float)assimpMesh->mBitangents[vertexIndex].z;

			m_texCoord.x = (float)assimpMesh->mTextureCoords[0][vertexIndex].x;
			m_texCoord.y = (float)assimpMesh->mTextureCoords[0][vertexIndex].y;
		}
	}
}