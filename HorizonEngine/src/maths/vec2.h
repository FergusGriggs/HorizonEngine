#pragma once

namespace hrzn::maths
{
    template<typename T>
    class Vec2
    {
    public:
        Vec2();
        Vec2(const T& val);
        Vec2(const T& _x, const T& _y);
        ~Vec2();

        // Operator overloads
        Vec2<T>        operator+(const Vec2<T>& vector) const;
        void           operator+=(const Vec2<T>& vector);
        Vec2<T>        operator+(const T scalar) const;
                       
        Vec2<T>        operator-(const Vec2<T>& vector) const;
        void           operator-=(const Vec2<T>& vector);
        Vec2<T>        operator-(const T scalar) const;
                       
        Vec2<T>        operator*(const Vec2<T>& vector) const;
        Vec2<T>        operator*(const T scalar) const;
        void           operator*=(const T scalar);
                       
        Vec2<T>        operator/(const Vec2<T>& vector) const;
        Vec2<T>        operator/(const T scalar) const;
        void           operator/=(const T scalar);

        bool           operator==(const Vec2<T>& vector) const;
        bool           operator==(const T scalar) const;

        T&             operator[](int index);

        // Member functions
        T              length() const;
        T              lengthSquared() const;
        void           normalise();
        T              dot(const Vec2<T>& vector) const;

        // Static functions
        static Vec2<T> normalise(const Vec2<T>& vector);
        static T       dot(const Vec2<T>& vector1, const Vec2<T>& vector2);

        static T       length(const Vec2<T>& vector);
        static T       lengthSquared(const Vec2<T>& vector);

        static T       dist(const Vec2<T>& vector1, const Vec2<T>& vector2);
        static T       distSquared(const Vec2<T>& vector1, const Vec2<T>& vector2);

        static Vec2<T> reflect(const Vec2<T>& incident, const Vec2<T>& normal);
        static Vec2<T> right(const Vec2<T>& vector);

        static Vec2<T> getRandomVector();
        static Vec2<T> getRandomDirection();
        static Vec2<T> lerp(const Vec2<T>& vector1, const Vec2<T>& Vec2, T delta);

        static Vec2<T> rotateAroundPoint(const Vec2<T>& toRotate, const Vec2<T>& point, double rotation, bool correctNDC = false, float aspect = 0.0f);

    public:
        T x, y;
    };

    typedef Vec2<float> Vec2f;
    typedef Vec2<int> Vec2i;
}

#include "vec2.inl"
