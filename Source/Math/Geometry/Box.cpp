#include "Box.h"

#include <algorithm>

template <typename T>
Box<T>::Box() = default;

template <typename T>
Box<T>::Box(const vec2<T>& pos, const vec2<T>& size) : Origin(pos), Size(size) {}

template <typename T>
void Box<T>::operator=(const Box<T>& other)
{
	Origin = other.Origin;
	Size = other.Size;
}

template <typename T>
void Box<T>::operator=(Box<T>&& other) noexcept
{
	std::swap(Origin, other.Origin);
	std::swap(Size, other.Size);
}

template <typename T>
Box<T>::~Box() = default;

#pragma region Instantiate
template Box<int>;
template Box<float>;
template Box<double>;
#pragma endregion


