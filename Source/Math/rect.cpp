#include "rect.h"

template <class T>
rect<T>::rect():pos(vec2<T>()),w(0.0f),h(0.0f)
{
}

template <class T>
rect<T>::rect(const float& x, const float& y, const float& w, const float& h):pos(vec2<T>(x,y)),w(w),h(h)
{
}

template <class T>
rect<T>::rect(const vec2<T>& pos, const float& w, const float& h):pos(pos),w(w),h(h)
{
}

template <class T>
rect<T>::rect(const rect& rect):pos(rect.pos),w(rect.w),h(rect.h)
{
}

template <class T>
rect<T>& rect<T>::operator=(const rect& rect)
{
    this->pos = rect.pos;
    this->h = rect.h;
    this->w = rect.w;
    return *this;
}

template <class T>
vec2<T> rect<T>::Center() const
{
    return vec2<T>(pos.x + w / 2.0f, pos.y + h / 2.0f);
}

template <class T>
T rect<T>::Left() const
{
    return pos.x; 
}

template <class T>
T rect<T>::Top() const
{
    return pos.y;
}

template <class T>
T rect<T>::Right() const
{
    return pos.x + w;
}

template <class T>
T rect<T>::Bottom() const
{
    return pos.y + h;
}

template rect<int>;
template rect<unsigned int>;
template rect<float>;
template rect<double>;
