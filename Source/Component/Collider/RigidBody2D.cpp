#include "RigidBody2D.h"

#include <DxLib.h>

#include "../TransformComponent.h"
#include "../../GameObject/Entity.h"
#include "../../Math/MathHelper.h"

RigidBody2D::RigidBody2D(const std::shared_ptr<Entity>& owner,
                         const vec2f& pos, const float& w, const float& h) :
	ICollider(owner),m_collider(pos, w, h),m_offset(w/2.f, h/2.f)
{
	auto transform = m_owner.lock()->GetComponent<TransformComponent>();
	m_collider.pos.x = transform->Pos.x - m_offset.x;
	m_collider.pos.y = transform->Pos.y - m_offset.y;
	destRect_ = m_collider;
	physicsUpdater_ = &RigidBody2D::NormalUpdate;
}

void RigidBody2D::NormalUpdate(const float& deltaTime)
{
	m_velocity.x = MathHelper::clamp(m_velocity.x, -maxVelocity_.x, maxVelocity_.x);
	m_velocity.y = MathHelper::clamp(m_velocity.y, -maxVelocity_.y, maxVelocity_.y);
}

void RigidBody2D::Update(float deltaTime)
{
	if (m_owner.expired())
		return;

	auto transform = m_owner.lock()->GetComponent<TransformComponent>();

	// collider_.pos = transform->GetLimitPosition(collider_.pos);
	m_collider.pos += (m_velocity + m_impactVeloctity)  * deltaTime;
	transform->Pos.x = m_collider.pos.x + m_offset.x ;
	transform->Pos.y = m_collider.pos.y + m_offset.y ;
}

void RigidBody2D::PhysicsUpdate(float deltaTime)
{
	(this->*physicsUpdater_)(deltaTime);
}

void RigidBody2D::Render()
{
#ifdef _DEBUG
    DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
    DxLib::DrawBoxAA(m_collider.pos.x, m_collider.pos.y, m_collider.pos.x + m_collider.w, m_collider.pos.y + m_collider.h, 0xff0000, 1);
    DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
#endif
}

COLLIDER_TYPE RigidBody2D::ColliderType()
{
	return COLLIDER_TYPE::RIGID_BODY_2D;
}

void RigidBody2D::ImpactUpdate(const float& deltaTime)
{
	if (m_impactVeloctity.x < 0)
	{
		m_impactVeloctity.x += impactResist_.x * deltaTime;
		if (m_impactVeloctity.x >= 0)
			m_impactVeloctity.x = 0;

	}
	else if (m_impactVeloctity.x > 0)
	{
		m_impactVeloctity.x -= impactResist_.x * deltaTime;
		if (m_impactVeloctity.x <= 0)
			m_impactVeloctity.x = 0;
	}
	else
	{
		physicsUpdater_ = &RigidBody2D::NormalUpdate;
		m_velocity.x = 0;
	}
		
}

void RigidBody2D::Init()
{
}

void RigidBody2D::SetMaxVelocity(const float& velX, const float& velY)
{
	maxVelocity_.x = velX;
	maxVelocity_.y = velY;
}

void RigidBody2D::Impact(const vec2f& impactValue)
{
	m_impactVeloctity = impactValue;
	physicsUpdater_ = &RigidBody2D::ImpactUpdate;
}



