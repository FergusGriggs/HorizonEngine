
#pragma once

#include <Assimp/mesh.h>

#include "../../maths/vec2.h"
#include "../../maths/vec3.h"
#include "../../maths/vec4.h"

namespace hrzn::gfx
{
	struct UnlitVertex
	{
		UnlitVertex();
		UnlitVertex(const maths::Vec3f& pos, const maths::Vec2f& texCoord);

		void fillUsingAssimpMesh(aiMesh* assimpMesh, int vertexIndex);

		maths::Vec3f m_pos;
		maths::Vec2f m_texCoord;
	};

	struct UnlitColourVertex : public UnlitVertex
	{
		UnlitColourVertex();
		UnlitColourVertex(const maths::Vec3f& pos, const maths::Vec2f& texCoord, const maths::Vec4f& colour);

		maths::Vec4f m_colour;
	};

	struct SimpleLitVertex
	{
		SimpleLitVertex();
		SimpleLitVertex(const maths::Vec3f& pos, const maths::Vec3f& normal, const maths::Vec2f& texCoord);

		void fillUsingAssimpMesh(aiMesh* assimpMesh, int vertexIndex);

		maths::Vec3f m_pos;
		maths::Vec3f m_normal;
		maths::Vec2f m_texCoord;
	};

	struct FancyLitVertex
	{
		FancyLitVertex();
		FancyLitVertex(const maths::Vec3f& pos, const maths::Vec3f& normal, const maths::Vec3f& tangent, const maths::Vec3f& bitangent, const maths::Vec2f& texCoord);

		void fillUsingAssimpMesh(aiMesh* assimpMesh, int vertexIndex);

		maths::Vec3f m_pos;
		maths::Vec3f m_normal;
		maths::Vec3f m_tangent;
		maths::Vec3f m_bitangent;
		maths::Vec2f m_texCoord;
	};

	struct SkinnedVertex
	{
		SkinnedVertex();
		SkinnedVertex(const maths::Vec3f& pos, const maths::Vec3f& normal, const maths::Vec3f& tangent, const maths::Vec3f& bitangent, const maths::Vec2f& texCoord,
			const maths::Vec4f& boneWeights, const maths::Vec4<uint8_t>& boneIndices);

		void fillUsingAssimpMesh(aiMesh* assimpMesh, int vertexIndex);

		maths::Vec3f m_pos;
		maths::Vec3f m_normal;
		maths::Vec3f m_tangent;
		maths::Vec3f m_bitangent;
		maths::Vec2f m_texCoord;

		maths::Vec4f         m_boneWeights;
		maths::Vec4<uint8_t> m_boneIndices;
	};
}
