#pragma once

#include "vec3.h"

#include <DirectXMath.h>

#include "random.h"

namespace hrzn::maths
{
    template<typename T>
    inline Vec3<T>::Vec3() :
        x((T)0),
        y((T)0),
        z((T)0)
    {
    }

    template<typename T>
    inline Vec3<T>::Vec3(const T& val) :
        x(val),
        y(val),
        z(val)
    {
    }

    template<typename T>
    inline Vec3<T>::Vec3(const T& _x, const T& _y, const T& _z) :
        x(_x),
        y(_y),
        z(_z)
    {
    }

    template<typename T>
    inline Vec3<T>::~Vec3()
    {
    }

    template<typename T>
    inline Vec3<T> Vec3<T>::operator+(const Vec3<T>& vector) const
    {
        return Vec3<T>(x + vector.x, y + vector.y, z + vector.z);
    }

    template<typename T>
    inline void Vec3<T>::operator+=(const Vec3<T>& vector)
    {
        x += vector.x;
        y += vector.y;
        z += vector.z;
    }

    template<typename T>
    inline Vec3<T> Vec3<T>::operator+(const T scalar) const
    {
        return Vec3<T>(x + scalar, y + scalar, z + scalar);
    }

    template<typename T>
    inline Vec3<T> Vec3<T>::operator-(const Vec3<T>& vector) const
    {
        return Vec3<T>(x - vector.x, y - vector.y, z - vector.z);
    }

    template<typename T>
    inline void Vec3<T>::operator-=(const Vec3<T>& vector)
    {
        x -= vector.x;
        y -= vector.y;
        z -= vector.z;
    }

    template<typename T>
    inline Vec3<T> Vec3<T>::operator-(const T scalar) const
    {
        return Vec3<T>(x - scalar, y - scalar, z - scalar);
    }

    template<typename T>
    inline Vec3<T> Vec3<T>::operator*(const Vec3<T>& vector) const
    {
        return Vec3<T>(x * vector.x, y * vector.y, z * vector.z);
    }

    template<typename T>
    inline Vec3<T> Vec3<T>::operator*(const T scalar) const
    {
        return Vec3<T>(x * scalar, y * scalar, z * scalar);
    }

    template<typename T>
    inline void Vec3<T>::operator*=(const T scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
    }

    template<typename T>
    inline Vec3<T> Vec3<T>::operator/(const T scalar) const
    {
        return Vec3<T>(x / scalar, y / scalar, z / scalar);
    }

    template<typename T>
    inline void Vec3<T>::operator/=(const T scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
    }

    template<typename T>
    inline bool Vec3<T>::operator==(const Vec3<T>& vector) const
    {
        return x == vector.x && y == vector.y && z == vector.z;
    }

    template<typename T>
    inline bool Vec3<T>::operator==(const T scalar) const
    {
        return x == scalar && y == scalar && z == scalar;
    }

    template<typename T>
    inline T& Vec3<T>::operator[](int index)
    {
        switch (index)
        {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        }

        return x;
    }

    template<typename T>
    inline T Vec3<T>::length() const
    {
        return sqrt(lengthSquared());
    }

    template<typename T>
    inline T Vec3<T>::lengthSquared() const
    {
        return x * x + y * y + z * z;
    }

    template<typename T>
    inline void Vec3<T>::normalise()
    {
        *this = Vec3<T>::normalise(*this);
    }

    template<typename T>
    inline T Vec3<T>::dot(const Vec3<T>& vector) const
    {
        return Vec3<T>::dot(*this, vector);
    }

    template<typename T>
    inline Vec3<T> Vec3<T>::cross(const Vec3<T>& vector)
    {
        return Vec3<T>::cross(*this, vector);
    }

    template<typename T>
    inline Vec3<float> Vec3<T>::createVec3f()
    {
        return Vec3<float>((float)x, (float)y, (float)z);
    }

    template<typename T>
    inline DirectX::XMVECTOR Vec3<T>::getAsXMVECTOR() const
    {
        return DirectX::XMVectorSet((float)x, (float)y, (float)z, 0.0f);
    }

    template<typename T>
    inline  DirectX::XMFLOAT3 Vec3<T>::getAsXMFLOAT3() const
    {
        return DirectX::XMFLOAT3((float)x, (float)y, (float)z);
    }

    template<typename T>
    inline Vec3<T> Vec3<T>::normalise(const Vec3<T>& vector)
    {
        T lengthSquared = vector.lengthSquared();

        if (lengthSquared < (T)0.0000001)
        {
            return Vec3<T>(0, -1, 0);
        }

        // Only sqrt for non-zero input
        T length = sqrt(lengthSquared);

        Vec3<T> normalisedVector;
        normalisedVector.x = vector.x / length;
        normalisedVector.y = vector.y / length;
        normalisedVector.z = vector.z / length;

        return normalisedVector;
    }

    template<typename T>
    inline T Vec3<T>::dot(const Vec3<T>& vector1, const Vec3<T>& vector2)
    {
        return vector1.x * vector2.x + vector1.y * vector2.y + vector1.z * vector2.z;
    }

    template<typename T>
    inline Vec3<T> Vec3<T>::cross(const Vec3<T>& vector1, const Vec3<T>& vector2)
    {
        return Vec3<T>(vector1.y * vector2.z - vector1.z * vector2.y,
            vector1.z * vector2.x - vector1.x * vector2.z,
            vector1.x * vector2.y - vector1.y * vector2.x);
    }

    template<typename T>
    inline T Vec3<T>::length(const Vec3<T>& vector)
    {
        return vector.length();
    }

    template<typename T>
    inline T Vec3<T>::lengthSquared(const Vec3<T>& vector)
    {
        return vector.lengthSquared();
    }

    template<typename T>
    inline T Vec3<T>::dist(const Vec3<T>& vector1, const Vec3<T>& vector2)
    {
        return sqrt(Vec3<T>::distSquared(vector1, vector2));
    }

    template<typename T>
    inline T Vec3<T>::distSquared(const Vec3<T>& vector1, const Vec3<T>& vector2)
    {
        Vec3<T> diffVector = vector1 - vector2;
        return diffVector.lengthSquared();
    }

    template<typename T>
    inline Vec3<T> Vec3<T>::reflect(const Vec3<T>& incident, const Vec3<T>& normal)
    {
        return incident - normal * 2.0f * Vec3<T>::dot(normal, incident);
    }

    template<typename T>
    inline Vec3<T> Vec3<T>::getRandomVector()
    {
        return Vec3<T>(random::negOneToOne<T>(), random::negOneToOne<T>(), random::negOneToOne<T>());
    }

    template<typename T>
    inline Vec3<T> Vec3<T>::getRandomDirection()
    {
        return Vec3<T>::normalise(Vec3<T>::getRandomVector());
    }

    template<typename T>
    inline Vec3<T> Vec3<T>::lerp(const Vec3<T>& vector1, const Vec3<T>& vector2, T delta)
    {
        return vector1 + (vector2 - vector1) * delta;
    }

    template<typename T>
    inline Vec3<T> Vec3<T>::min(const Vec3<T>& vector1, const Vec3<T>& vector2)
    {
        return Vec3<T>(fminf(vector1.x, vector2.x), fminf(vector1.y, vector2.y));
    }

    template<typename T>
    inline Vec3<T> Vec3<T>::max(const Vec3<T>& vector1, const Vec3<T>& vector2)
    {
        return Vec3<T>(fmaxf(vector1.x, vector2.x), fmaxf(vector1.y, vector2.y), fmaxf(vector1.z, vector2.z));
    }

    template<typename T>
    inline Vec3<T> Vec3<T>::abs(const Vec3<T>& vector)
    {
        return Vec3<T>(fabsf(vector.x), fabsf(vector.y), fabsf(vector.z));
    }

    template<typename T>
    inline Vec3<T> Vec3<T>::makeFromXMVECTOR(const DirectX::XMVECTOR& vector)
    {
        return Vec3<T>((T)DirectX::XMVectorGetX(vector), (T)DirectX::XMVectorGetY(vector), (T)DirectX::XMVectorGetZ(vector));
    }

    template<typename T>
    inline Vec3<T> Vec3<T>::makeFromXMFLOAT3(const DirectX::XMFLOAT3& float3)
    {
        return Vec3<T>(float3.x, float3.y, float3.z);
    }
}
