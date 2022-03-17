#pragma once

#include "vec2.h"

#include <math.h>
#include <algorithm>

#include "random.h"

namespace hrzn::maths
{
    template<typename T>
    inline Vec2<T>::Vec2() :
        x((T)0),
        y((T)0)
    {
    }

    template<typename T>
    inline Vec2<T>::Vec2(const T& val) :
        x(val),
        y(val)
    {
    }

    template<typename T>
    inline Vec2<T>::Vec2(const T& _x, const T& _y) :
        x(_x),
        y(_y)
    {
    }

    template<typename T>
    inline Vec2<T>::~Vec2()
    {
    }

    template<typename T>
    inline Vec2<T> Vec2<T>::operator+(const Vec2<T>& vector) const
    {
        return Vec2<T>(x + vector.x, y + vector.y);
    }

    template<typename T>
    inline void Vec2<T>::operator+=(const Vec2<T>& vector)
    {
        x += vector.x;
        y += vector.y;
    }

    template<typename T>
    inline Vec2<T> Vec2<T>::operator+(const T scalar) const
    {
        return Vec2<T>(x + scalar, y + scalar);
    }

    template<typename T>
    inline Vec2<T> Vec2<T>::operator-(const Vec2<T>& vector) const
    {
        return Vec2<T>(x - vector.x, y - vector.y);
    }

    template<typename T>
    inline void Vec2<T>::operator-=(const Vec2<T>& vector)
    {
        x -= vector.x;
        y -= vector.y;
    }

    template<typename T>
    inline Vec2<T> Vec2<T>::operator-(const T scalar) const
    {
        return Vec2<T>(x - scalar, y - scalar);
    }

    template<typename T>
    inline Vec2<T> Vec2<T>::operator*(const Vec2<T>& vector) const
    {
        return Vec2<T>(x * vector.x, y * vector.y);
    }

    template<typename T>
    inline Vec2<T> Vec2<T>::operator*(const T scalar) const
    {
        return Vec2<T>(x * scalar, y * scalar);
    }

    template<typename T>
    inline void Vec2<T>::operator*=(const T scalar)
    {
        x *= scalar;
        y *= scalar;
    }

    template<typename T>
    inline Vec2<T> Vec2<T>::operator/(const Vec2<T>& vector) const
    {
        return Vec2<T>(x / vector.x, y / vector.y);
    }

    template<typename T>
    inline Vec2<T> Vec2<T>::operator/(const T scalar) const
    {
        return Vec2<T>(x / scalar, y / scalar);
    }

    template<typename T>
    inline void Vec2<T>::operator/=(const T scalar)
    {
        x /= scalar;
        y /= scalar;
    }

    template<typename T>
    inline bool Vec2<T>::operator==(const Vec2<T>& vector) const
    {
        return x == vector.x && y == vector.y;
    }

    template<typename T>
    inline bool Vec2<T>::operator==(const T scalar) const
    {
        return x == scalar && y == scalar;
    }

    template<typename T>
    inline T& Vec2<T>::operator[](int index)
    {
        switch (index)
        {
        case 0:
            return x;
        case 1:
            return y;
        }

        return x;
    }

    template<typename T>
    inline bool Vec2<T>::operator<(const Vec2<T>& b) const
    {
        if (y < b.y)
        {
            // y < b.y
            return true;
        }
        else if (y == b.y)
        {
            if (x < b.x)
            {
                return true;
            }
            else if (x == b.x)
            {
                // completely equal
                return false;
            }
            else
            {
                return false;
            }
        }
        else
        {
            // y >= b.y
            return false;
        }
    }

    template<typename T>
    inline T Vec2<T>::length() const
    {
        return sqrt(lengthSquared());
    }

    template<typename T>
    inline T Vec2<T>::lengthSquared() const
    {
        return x * x + y * y;
    }

    template<typename T>
    inline void Vec2<T>::normalise()
    {
        *this = Vec2<T>::normalise(*this);
    }

    template<typename T>
    inline T Vec2<T>::dot(const Vec2<T>& vector) const
    {
        return Vec2<T>::dot(*this, vector);
    }

    template<typename T>
    inline Vec2<T> Vec2<T>::normalise(const Vec2<T>& vector)
    {
        T lengthSquared = vector.lengthSquared();

        if (lengthSquared < (T)0.001)
        {
            return Vec2<T>(0, -1);
        }

        // Only sqrt for non-zero input
        T length = sqrt(lengthSquared);

        Vec2<T> normalisedVector;
        normalisedVector.x = vector.x / length;
        normalisedVector.y = vector.y / length;

        return normalisedVector;
    }

    template<typename T>
    inline T Vec2<T>::dot(const Vec2<T>& vector1, const Vec2<T>& vector2)
    {
        return (vector1.x * vector2.x) + (vector1.y * vector2.y);
    }

    template<typename T>
    inline T Vec2<T>::length(const Vec2<T>& vector)
    {
        return vector.length();
    }

    template<typename T>
    inline T Vec2<T>::lengthSquared(const Vec2<T>& vector)
    {
        return vector.lengthSquared();
    }

    template<typename T>
    inline T Vec2<T>::dist(const Vec2<T>& vector1, const Vec2<T>& vector2)
    {
        return sqrt(Vec2<T>::distSquared(vector1, vector2));
    }

    template<typename T>
    inline T Vec2<T>::distSquared(const Vec2<T>& vector1, const Vec2<T>& vector2)
    {
        Vec2<T> diffVector = vector1 - vector2;
        return diffVector.lengthSquared();
    }

    template<typename T>
    inline Vec2<T> Vec2<T>::reflect(const Vec2<T>& incident, const Vec2<T>& normal)
    {
        return incident - normal * 2.0f * Vec2<T>::dot(normal, incident);
    }

    template<typename T>
    inline Vec2<T> Vec2<T>::right(const Vec2<T>& vector)
    {
        return Vec2<T>(-vector.y, vector.x);
    }

    template<typename T>
    inline Vec2<T> Vec2<T>::getRandomVector()
    {
        return Vec2<T>(random::negOneToOne<T>(), random::negOneToOne<T>());
    }

    template<typename T>
    inline Vec2<T> Vec2<T>::getRandomDirection()
    {
        return Vec2<T>::normalise(Vec2<T>::getRandomVector());
    }

    template<typename T>
    inline Vec2<T> Vec2<T>::lerp(const Vec2<T>& vector1, const Vec2<T>& vector2, T delta)
    {
        return vector1 + (vector2 - vector1) * delta;
    }

    template<typename T>
    inline Vec2<T> Vec2<T>::min(const Vec2<T>& vector1, const Vec2<T>& vector2)
    {
        return Vec2<T>(std::min(vector1.x, vector2.x), std::min(vector1.y, vector2.y));
    }

    template<typename T>
    inline Vec2<T> Vec2<T>::max(const Vec2<T>& vector1, const Vec2<T>& vector2)
    {
        return Vec2<T>(std::max(vector1.x, vector2.x), std::max(vector1.y, vector2.y));
    }

    template<typename T>
    inline Vec2<T> Vec2<T>::rotateAroundPoint(const Vec2<T>& toRotate, const Vec2<T>& point, double rotation, bool correctNDC, float aspect)
    {
        Vec2<T> offset = toRotate - point;
        Vec2<T> rotatedOffset;

        if (correctNDC)
        {
            offset.y /= aspect;
        }

        rotatedOffset.x = (T)(cos(rotation) * (double)offset.x - sin(rotation) * (double)offset.y);
        rotatedOffset.y = (T)(sin(rotation) * (double)offset.x + cos(rotation) * (double)offset.y);

        if (correctNDC)
        {
            rotatedOffset.y *= aspect;
        }

        return point + rotatedOffset;
    }
}
