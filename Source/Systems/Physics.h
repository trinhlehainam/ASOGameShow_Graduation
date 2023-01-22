#pragma once
#include <memory>
#include <vector>

#include "../Component/Collider/RigidBody2D.h"

template <typename T>
struct vec2;
using vec2f = vec2<float>;

template <typename T>
struct rect;
using rectf = rect<float>;

class ICollider;
struct HitInfo;

// Dynamic Singleton
class Physics
{
public:
    static void Create();
    static void Destroy();
    static Physics& Instance();
public:
    static void AddCollider(const std::shared_ptr<ICollider>& collider);
    static void ApplyForce(float DeltaTime);
    static void Update(float DeltaTime);
    static void PlatformResolution(float DeltaTime);
    static void Render();

    static bool RayCast(const vec2f& origin, const vec2f& dir, float maxDistance);
    static bool CheckSweptAABB(HitInfo& hitInfo, const rectf& main, const vec2f& vec, const rectf& target,
                               float deltaTime);
    static bool RayCast(HitInfo& hitInfo, const vec2f& ray_point, const vec2f& ray_dir, const rectf& target);
    static std::shared_ptr<RigidBody2D>& AddRigidBody(const std::shared_ptr<Entity>& owner, const vec2f& pos, const float&w, const float& h);
private:
    Physics();
    ~Physics();

private:
    // Don't allow copy and move semantics
    Physics(const Physics&);
    Physics(Physics&&) noexcept;
    void operator =(const Physics&);
    void operator =(Physics&&) noexcept;
private:
    std::vector<std::shared_ptr<ICollider>> m_colliders;
    std::vector<std::shared_ptr<RigidBody2D>> m_actorBodies;
	 
    static Physics* m_instance;
};
