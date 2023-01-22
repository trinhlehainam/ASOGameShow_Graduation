#include "BoxCollider.h"

#include <DxLib.h>

#include "../../GameObject/Entity.h"
#include "../TransformComponent.h"

BoxCollider::BoxCollider(const std::shared_ptr<Entity>& owner):ICollider(owner)
{
}

BoxCollider::BoxCollider(const std::shared_ptr<Entity>& owner, const rectf& boxInfo):ICollider(owner),m_collider(boxInfo)
{
    const auto& transform = owner->GetComponent<TransformComponent>();
    ICollider::SetOffset(m_collider.pos - transform->Pos);
}

BoxCollider::~BoxCollider()
{
}

void BoxCollider::SetOrigin(const vec2f& origin)
{
    m_collider.pos = origin;
    const auto& transform = GetOwner()->GetComponent<TransformComponent>();
    ICollider::SetOffset(m_collider.pos - transform->Pos);
}

void BoxCollider::SetSize(const vec2f& size)
{
    m_collider.w = size.x;
    m_collider.h = size.y;
}

vec2f BoxCollider::GetOrigin() const
{
    return m_collider.Center();
}

vec2f BoxCollider::GetSize() const
{
    return vec2f(m_collider.w, m_collider.h);
}

rectf BoxCollider::GetCollider() const
{
    return m_collider;
}

void BoxCollider::Init()
{
}

void BoxCollider::Update(float deltaTime_s)
{
    const auto& transform = IComponent::GetOwner()->GetComponent<TransformComponent>();
    m_collider.pos = transform->Pos + m_offset;
}

void BoxCollider::Render()
{
#ifdef _DEBUG
    // DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
    DxLib::DrawBoxAA(m_collider.pos.x, m_collider.pos.y, m_collider.pos.x + m_collider.w, m_collider.pos.y + m_collider.h, 0xff0000, 1);
    // DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
#endif
}

COLLIDER_TYPE BoxCollider::ColliderType()
{
    return COLLIDER_TYPE::BOX;
}
