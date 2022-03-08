#pragma once

#include <DirectXMath.h>

// Purely just a storage class at the moment

namespace Firelight::Maths
{
    template<typename T>
    class Vec4
    {
    public:
        Vec4();
        Vec4(const T& val);
        Vec4(const T& _x, const T& _y, const T& _z, const T& _w);
        ~Vec4();

        T& operator[](int index);

        // XMVECTOR
        DirectX::XMVECTOR getAsXMVECTOR() const;
        DirectX::XMFLOAT4 getAsXMFLOAT4() const;

        static Vec4<T>    makeFromXMVECTOR(const DirectX::XMVECTOR& vector);
        static Vec4<T>    makeFromXMFLOAT4(const DirectX::XMFLOAT4& float4);

    public:
        T x, y, z, w;
    };

    typedef Vec4<float> Vec4f;
    typedef Vec4<int> Vec4i;
}

#include "vec4.inl"
