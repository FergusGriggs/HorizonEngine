/*

Implementations of Octree member functions.

Copyright (C) 2011  Tao Ju

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public License
(LGPL) as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#pragma once

#include <Windows.h>
#include <vector>

#include "../../../maths/vec3.h"

#include "qef.h"
#include "../../../graphics/data/vertex.h"

namespace hrzn::scene::terrain
{
	enum class OctreeNodeType
	{
		eNone,
		eInternal,
		ePseudo,
		eLeaf,
	};


	struct OctreeDrawInfo
	{
		OctreeDrawInfo()
			: m_index(-1)
			, m_corners(0)
		{
		}

		int				m_index;
		int				m_corners;
		maths::Vec3f	m_position;
		maths::Vec3f	m_averageNormal;
		svd::QefData	m_qef;
	};


	class OctreeNode
	{
	public:

		OctreeNode()
			: m_type(OctreeNodeType::eNone)
			, m_min(0, 0, 0)
			, m_size(0)
			, m_drawInfo(nullptr)
		{
			for (int i = 0; i < 8; i++)
			{
				m_children[i] = nullptr;
			}
		}

		OctreeNode(const OctreeNodeType type)
			: m_type(type)
			, m_min(0, 0, 0)
			, m_size(0)
			, m_drawInfo(nullptr)
		{
			for (int i = 0; i < 8; i++)
			{
				m_children[i] = nullptr;
			}
		}

		OctreeNodeType  m_type;
		maths::Vec3i    m_min;
		int				m_size;
		OctreeNode*     m_children[8];
		OctreeDrawInfo* m_drawInfo;
	};

	// ----------------------------------------------------------------------------

	OctreeNode* BuildOctree(const maths::Vec3i& min, const int size, const float threshold);
	void DestroyOctree(OctreeNode* node);
	void GenerateMeshFromOctree(OctreeNode* node, std::vector<gfx::Vertex>& vertexBuffer, std::vector<DWORD>& indexBuffer);

	// ----------------------------------------------------------------------------
}
