#include "MathHelper.h"

#include <cmath>
#include <algorithm>
#include <array>

#include "rect.h"
#include "../Math/line2.h"
#include "../Math/segment2.h"
#include "Geometry/Circle.h"
#include "Geometry/Triangle.h"

namespace
{
	// Value of 2 points in one axis
	struct range
	{
		float min, max;
	};
}

namespace MathHelper
{
	const float kBiasF = 0.001f;
	const double kBiasD = 0.0001;

	const float kMsToSecond = 1 / 1000.0f;
}

namespace MathHelper
{
	float randf(float num)
	{
		return static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / num));
	}

	float randf(float min, float max)
	{
		return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
	}

	template<typename T>
	bool isEqual(T a, T b)
	{
		return a == b;
	}

	template<typename T>
	T clamp(T value, T min, T max)
	{
		return std::max(min,std::min(max, value));
	}

#pragma region Specialization
	template <>
	bool isEqual(float a, float b)
	{
		return std::fabsf(a - b) <= kBiasF;
	}

	template <>
	bool isEqual(double a, double b)
	{
		return std::abs(a - b) <= kBiasD;
	}

#pragma endregion


#pragma region InstantiateFuncTemplate
	template bool isEqual(int, int);
	template bool isEqual(unsigned int, unsigned int);
	template bool isEqual(short, short);

	template float clamp(float, float, float);
	template double clamp(double, double, double);
	
#pragma endregion
};

namespace MathHelper
{
	bool isEquivalent(const line2& a, const line2& b)
	{
		if (!isParallelVec(a.dir, b.dir))
			return false;

		return isParallelVec(a.base - b.base, a.dir);
	}

	bool isOnSameSide(const line2& l, const segment2& s)
	{
		auto orthoVec = orthogonalVec(l.dir);
		return dot(orthoVec, s.a - l.base) * dot(orthoVec, s.b - l.base) > 0.0f;
	}

	range projectSegmentRange(const segment2& s, const vec2f& onto)
	{
		auto ontoUnit = onto / std::sqrt(dot(onto,onto));

		range r{ dot(s.a, onto) , dot(s.a, onto) };
		if (r.min > r.max) std::swap(r.min, r.max);

		return r;
	}

	bool isOverlap(const range& a, const range& b)
	{
		return isOverlap(a.min, a.max, b.min, b.max);
	}

	bool isOverlap(const position2f& p, const Circle& c)
	{
		vec2f distanceVec = c.Center - p;
		return dot(distanceVec, distanceVec) <= c.Radius * c.Radius;
	}

	bool isOverlap(const position2f& p, const rectf& r)
	{
		float left = r.pos.x;
		float right = r.pos.x + r.w;
		float top = r.pos.y;
		float bottom = r.pos.y + r.h;

		return left <= p.x && p.x <= right &&
			   top <= p.y && p.y <= bottom;
	}

	bool isOverlap(const Circle& c, const position2f& p)
	{
		return isOverlap(p,c);
	}

	bool isOverlap(const Circle& a, const Circle& b)
	{
		auto d = a.Center - b.Center;
		auto r = a.Radius + b.Radius;
		return d * d <= r * r;
	}

	bool isOverlap(const Circle& c, const rectf& r)
	{
		auto closet_point = clampVec(c.Center, r.pos, r.pos + vec2f(r.w, r.h));
		auto d = closet_point - c.Center;
		return d * d <= c.Radius * c.Radius;
	}

	bool isOverlap(const rectf& r, const Circle& c)
	{
		return isOverlap(c, r);
	}

	bool isOverlap(const rectf& a, const rectf& b)
	{
		auto aLeft = a.pos.x;
		auto aRight = a.pos.x + a.w;
		auto aTop = a.pos.y;
		auto aBottom = a.pos.y + a.h;

		auto bLeft = b.pos.x;
		auto bRight = b.pos.x + b.w;
		auto bTop = b.pos.y;
		auto bBottom = b.pos.y + b.h;

		return isOverlap(aLeft, aRight, bLeft, bRight) && isOverlap(aTop, aBottom, bTop, bBottom);
	}

	bool isOverlap(const Triangle& a, const Triangle& b)
	{
		// Create segments from triangle
		std::array<segment2, 3> a_segments, b_segments;
		for (int i = 0; i < 3; ++i)
		{
			auto i_next = (i + 1) % 3;
			a_segments[i].a = a.Point[i];
			a_segments[i].b = a.Point[i_next];
		}
		for (int i = 0; i < 3; ++i)
		{
			auto i_next = (i + 1) % 3;
			b_segments[i].a = b.Point[i];
			b_segments[i].b = b.Point[i_next];
		}
		//

		for (const auto& a_segment : a_segments)
		{
			for (const auto& b_segment : b_segments)
			{
				if (isOverlap(a_segment, b_segment))
					return true;
			}
		}

		return false;
	}

	bool isOverlap(const line2& a, const line2& b)
	{
		if (!isParallelVec(a.dir, b.dir))
			return true;
		return isEquivalent(a, b);
	}

	bool isOverlap(const line2& line, const Circle& cir)
	{
		vec2f proj = projectVec(cir.Center - line.base, line.dir);
		position2f nearest = line.base + proj;
		return isOverlap(nearest, cir);
	}

	bool isOverlap(const line2& l, const rectf& r)
	{
		vec2f n = orthogonalVec(l.dir);

		vec2f l1 = r.pos - l.base;
		vec2f l2 = vec2f{ r.pos.x + r.w, r.pos.y } - l.base ;
		vec2f l3 = (r.pos + vec2f(r.w, r.h)) - l.base ;
		vec2f l4 = vec2f{ r.pos.x, r.pos.y + r.h } - l.base ;

		auto dot1 = dot(l1, n);
		auto dot2 = dot(l2, n);
		auto dot3 = dot(l3, n);
		auto dot4 = dot(l4, n);

		return dot1 * dot2 <= 0.0f ||
			   dot2 * dot3 <= 0.0f ||
			   dot3 * dot4 <= 0.0f ||
			   dot4 * dot1 <= 0.0f;
	}

	bool isOverlap(const segment2& a, const segment2& b)
	{
		line2 axisA{ a.a, a.b - a.a };
		if(isOnSameSide(axisA, b))
			return false;
		
		line2 axisB{ b.a, b.b - b.a };
		if (isOnSameSide(axisB, a))
			return false;

		if (!isParallelVec(axisA.dir, axisB.dir))
			return true;

		return isOverlap(projectSegmentRange(a, axisA.dir), projectSegmentRange(b, axisA.dir));
	}

	bool isOverlap(const segment2& s, const Circle& c)
	{
		if (isOverlap(s.a, c)) return true;
		if (isOverlap(s.b, c)) return true;

		vec2f lc = c.Center - s.a;
		vec2f d = s.b - s.a;
		vec2f p = projectVec(lc, d);
		position2f nearest = s.a + p;

		if (!isOverlap(nearest, c)) return false;
		if (dot(d, p) < 0) return false;

		return dot(p,p) <= dot(d,d);
	}

	bool isOverlap(const segment2& s, const rectf& r)
	{
		range segX{ s.a.x,s.b.x };
		if (segX.min > segX.max) std::swap(segX.min, segX.max);
		if (!isOverlap(segX.min, segX.max, r.pos.x, r.pos.x + r.w)) return false;

		range segY{ s.a.y,s.b.y };
		if (segY.min > segY.max) std::swap(segY.min, segY.max);
		return isOverlap(segY.min, segY.max, r.pos.y, r.pos.y + r.h);
	}

	bool isOverlap(float minA, float maxA, float minB, float maxB)
	{
		return maxA >= minB && minA <= maxB;
	}

}
