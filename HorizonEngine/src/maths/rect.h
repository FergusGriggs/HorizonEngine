#pragma once

#include "vec2.h"

namespace hrzn::maths
{
    template<typename T>
    class Rect
    {
    public:
        Rect();
        Rect(const T& _x, const T& _y, const T& _w, const T& _h);

        Rect<T> createNDCRectFromPixelRect(const maths::Vec2f& dimensions) const;
        bool    isValid() const;

        Vec2<T> getCentre() const;
        Vec2<T> getCentreNDC() const;

        static Rect<T> createNDCRectInWorldRect(const Rect<T>& smallRect, const Rect<T>& worldRect);

    public:
        T x, y, w, h;
    };

    typedef Rect<float> Rectf;
    typedef Rect<int> Recti;
}

#include "rect.inl"
