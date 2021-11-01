#pragma once

#include <DirectXMath.h>

using namespace DirectX;

namespace hrzn::physics::collision
{
    static XMVECTOR rayPlaneIntersect(const XMVECTOR& rayOrigin, const XMVECTOR& rayDirection, const XMVECTOR& planeNormal, const XMVECTOR& planePoint)
	{
		XMVECTOR diff = rayOrigin - planePoint;
		return (diff + planePoint) + rayDirection * (-XMVector3Dot(diff, planeNormal) / XMVector3Dot(rayDirection, planeNormal));
	}

	static bool rayFaceIntersect(const XMVECTOR& vertex1, const XMVECTOR& vertex2, const XMVECTOR& vertex3, const XMVECTOR& rayOrigin, const XMVECTOR& rayDirection, float* rayDistance)
	{
		// Face normal
		XMVECTOR faceNormal = XMVector3Normalize(XMVector3Cross(vertex2 - vertex1, vertex3 - vertex1));

		// Plane intersect point
		XMVECTOR planeIntersectPoint = rayPlaneIntersect(rayOrigin, rayDirection, faceNormal, vertex1);

		// Check if the ray could hit the face
		if (XMVectorGetX(XMVector3Dot(planeIntersectPoint - rayOrigin, rayDirection)) >= 0.0)
		{
			// Work out barrycentric coordinates to check if point is in face
			XMVECTOR v0 = vertex2 - vertex1;
			XMVECTOR v1 = vertex3 - vertex1;
			XMVECTOR v2 = planeIntersectPoint - vertex1;

			float d00 = XMVectorGetX(XMVector3Dot(v0, v0));
			float d01 = XMVectorGetX(XMVector3Dot(v0, v1));
			float d11 = XMVectorGetX(XMVector3Dot(v1, v1));
			float d20 = XMVectorGetX(XMVector3Dot(v2, v0));
			float d21 = XMVectorGetX(XMVector3Dot(v2, v1));
			float denom = d00 * d11 - d01 * d01;
			float v = (d11 * d20 - d01 * d21) / denom;
			float w = (d00 * d21 - d01 * d20) / denom;
			float u = 1.0f - v - w;

			if ((u >= 0.0f) && (v >= 0.0f) && (u + v <= 1.0f))
			{
				*rayDistance = XMVectorGetX(XMVector3Length(rayOrigin - planeIntersectPoint));
				return true;
			}
		}

		return false;
	}
}