#pragma once
#include "../vec2.h"

template <typename T>
struct Box
{
	Box();
	Box(const vec2<T>& pos, const vec2<T>& size);
	Box(const Box&) = default;
	Box(Box&&) noexcept = default;
	void operator = (const Box&);
	void operator = (Box&&) noexcept;
	~Box();

	vec2<T> Origin;
	vec2<T> Size;
};

using Boxi = Box<int>;
using Boxf = Box<float>;
using Boxd = Box<double>;

