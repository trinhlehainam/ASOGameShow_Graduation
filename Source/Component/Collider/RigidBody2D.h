#pragma once

#include <memory>
#include <string>
#include "ICollider.h"
#include "../../Math/rect.h"

class Entity;

/// <param name="owner"></param>
/// <param name="pos"></param>
/// <param name="w"></param>
/// <param name="h"></param>
/// <returns></returns>
class RigidBody2D:
    public ICollider
{
private:
    friend class Physics;
    vec2f maxVelocity_ = vec2f(700, 700);

    using Update_t = void(RigidBody2D::*)(const float&);
    Update_t physicsUpdater_;

    void NormalUpdate(const float&);
    void ImpactUpdate(const float&);

    vec2f impactResist_ = vec2f(1000,1000);
    vec2f m_impactVeloctity = vec2f(0, 0);
public:
    rectf m_collider;
    rectf destRect_;
    vec2f m_velocity = vec2f(0, 0);
    vec2f m_offset;
	
    void Init() override;
    void Update(float deltaTime) override;
    void PhysicsUpdate(float deltaTime);
    void Render() override;
    COLLIDER_TYPE ColliderType() override;
    void SetMaxVelocity(const float& velX, const float& velY);
    void Impact(const vec2f& impactValue);
	
    RigidBody2D(const std::shared_ptr<Entity>& owner, const vec2f& pos, const float&w, const float& h);

    ~RigidBody2D() = default;
};