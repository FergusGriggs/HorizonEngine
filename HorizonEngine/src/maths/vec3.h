#pragma once

#include <DirectXMath.h>

namespace hrzn::maths
{
    template<typename T>
    class Vec3
    {
    public:
        Vec3();
        Vec3(const T& val);
        Vec3(const T& _x, const T& _y, const T& _z);
        ~Vec3();

        // Operator overloads
        Vec3<T>           operator+(const Vec3<T>& vector) const;
        void              operator+=(const Vec3<T>& vector);
        Vec3<T>           operator+(const T scalar) const;

        Vec3<T>           operator-(const Vec3<T>& vector) const;
        void              operator-=(const Vec3<T>& vector);
        Vec3<T>           operator-(const T scalar) const;

        Vec3<T>           operator*(const Vec3<T>& vector) const;
        Vec3<T>           operator*(const T scalar) const;
        void              operator*=(const T scalar);

        Vec3<T>           operator/(const T scalar) const;
        void              operator/=(const T scalar);

        bool              operator==(const Vec3<T>& vector) const;
        bool              operator==(const T scalar) const;

        T&                operator[](int index);

        // Member functions
        T                 length() const;
        T                 lengthSquared() const;
        void              normalise();
        T                 dot(const Vec3<T>& vector) const;
        Vec3<T>           cross(const Vec3<T>& vector);

        // XMVECTOR
        DirectX::XMVECTOR getAsXMVECTOR() const;
        DirectX::XMFLOAT3 getAsXMFLOAT3() const;

        // Static functions
        static Vec3<T>    normalise(const Vec3<T>& vector);
        static T          dot(const Vec3<T>& vector1, const Vec3<T>& vector2);
        static Vec3<T>    cross(const Vec3<T>& vector1, const Vec3<T>& vector2);
                          
        static T          length(const Vec3<T>& vector);
        static T          lengthSquared(const Vec3<T>& vector);
                          
        static T          dist(const Vec3<T>& vector1, const Vec3<T>& vector2);
        static T          distSquared(const Vec3<T>& vector1, const Vec3<T>& vector2);
                          
        static Vec3<T>    reflect(const Vec3<T>& incident, const Vec3<T>& normal);
                          
        static Vec3<T>    getRandomVector();
        static Vec3<T>    getRandomDirection();
        static Vec3<T>    lerp(const Vec3<T>& vector1, const Vec3<T>& Vec3, T delta);

        // XMVECTOR
        static Vec3<T>    makeFromXMVECTOR(const DirectX::XMVECTOR& vector);
        static Vec3<T>    makeFromXMFLOAT3(const DirectX::XMFLOAT3& float3);

    public:
        T x, y, z;
    };

    typedef Vec3<float> Vec3f;
    typedef Vec3<int> Vec3i;
}

#include "vec3.inl"
