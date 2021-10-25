#pragma once

#include <fstream>

#include <DirectXMath.h>

namespace hrzn::utils::io_helpers
{
	using namespace DirectX;

	static XMFLOAT3 readFloat3(std::ifstream& stream)
	{
		XMFLOAT3 vector;
		stream >> vector.x;
		stream >> vector.y;
		stream >> vector.z;
		return vector;
	}

	static XMFLOAT4 readFloat4(std::ifstream& stream)
	{
		XMFLOAT4 vector;
		stream >> vector.x;
		stream >> vector.y;
		stream >> vector.z;
		stream >> vector.w;
		return vector;
	}

	static void writeFloat3(const XMFLOAT3& float3, std::ofstream& stream)
	{
		stream << float3.x << ' ' << float3.y << ' ' << float3.z << ' ';
	}

	static void writeFloat3(const XMVECTOR& float3, std::ofstream& stream)
	{
		stream << XMVectorGetX(float3) << ' ' << XMVectorGetY(float3) << ' ' << XMVectorGetZ(float3) << ' ';
	}

	static void writeFloat4(const XMFLOAT4& float4, std::ofstream& stream)
	{
		stream << float4.x << ' ' << float4.y << ' ' << float4.z << ' ' << float4.w << ' ';
	}

	static void writeFloat4(const XMVECTOR& float4, std::ofstream& stream)
	{
		stream << XMVectorGetX(float4) << ' ' << XMVectorGetY(float4) << ' ' << XMVectorGetZ(float4) << ' ' << XMVectorGetW(float4) << ' ';
	}
}
