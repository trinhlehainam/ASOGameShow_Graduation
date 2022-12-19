#pragma once
#include "vec2.h"

template <class T>
struct rect
{
    vec2<T> pos;
    float w, h;
    
    rect();
    rect(const float& x, const float& y, const float& w, const float& h);
    rect(const vec2<T>& pos, const float& w, const float& h);
    rect(const rect& rect);
    rect(rect&&) = default;
    rect& operator = (const rect& rect);
    rect& operator = (rect&&) = default;

    vec2<T> Center() const;
    float Left() const;
    float Top() const;
    float Right() const;
    float Bottom() const;
};

using recti = rect<int>;
using rectu = rect<unsigned int>;
using rectf = rect<float>;
using rectd = rect<double>;
