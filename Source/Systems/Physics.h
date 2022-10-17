#pragma once
#include <memory>
#include <vector>

template<typename T>
struct vec2;

using vec2f = vec2<float>;

class ICollider;

// Dynamic Singleton
class Physics
{
public:
	static void Create();
	static void Destroy();
	static Physics& Instance();
public:
	static bool RayCast(const vec2f& origin, const vec2f& dir, float maxDistance);
	static void AddCollider(const std::shared_ptr<ICollider>& collider);

private:
	Physics();
	~Physics();

private:
	// Don't allow copy and move semantics
	Physics(const Physics&);
	Physics(Physics&&) noexcept;
	void operator = (const Physics&);
	void operator = (Physics&&) noexcept;
private:
	 std::vector<std::weak_ptr<ICollider>> m_colliders;
	 static Physics* m_instance;
};

