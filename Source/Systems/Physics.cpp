#include "Physics.h"

#include <algorithm>
#include <DxLib.h>

#include "../Utilities/MacroHelper.h"

#include "../Math/MathHelper.h"
#include "../Math/vec2.h"
#include "../Math/segment2.h"
#include "../Math/rect.h"

#include "../Component/Collider/BoxCollider.h"
#include "../Component/Collider/CircleCollider.h"
#include "../Math/HitInfo.h"

GenerateDynamicSingleton(Physics);

namespace
{
    template <typename T>
    void ProcessRemoveCollider(std::vector<std::shared_ptr<T>>& container)
    {
        container.erase(std::remove_if(container.begin(), container.end(), [](std::shared_ptr<T>& collider)
                        {
                            return !collider->IsOwnerExist();
                        }),
                        container.end());
    }

    template <typename T>
    void ProcessRemoveCollider(std::vector<T>& container)
    {
        container.erase(std::remove_if(container.begin(), container.end(), [](T& collider)
                        {
                            return !collider.IsOwnerExist();
                        }),
                        container.end());
    }
}

bool Physics::RayCast(const vec2f& origin, const vec2f& dir, float maxDistance)
{
    segment2 seg{origin, origin + unitVec(dir) * maxDistance};
    for (const auto& pCollider : m_instance->m_colliders)
    {
        auto pCollider_checked = pCollider;
        auto collider_type = pCollider_checked->ColliderType();
        if (collider_type == COLLIDER_TYPE::BOX)
            {
                auto boxCollider = std::dynamic_pointer_cast<BoxCollider>(pCollider_checked);
                if (MathHelper::isOverlap(seg, boxCollider->GetCollider()))
                    return true;
            }
        else if (collider_type ==  COLLIDER_TYPE::CIRCLE)
            {
                auto cirCollider = std::dynamic_pointer_cast<CircleCollider>(pCollider_checked);
                if (MathHelper::isOverlap(seg, cirCollider->GetCollider()))
                    return true;
            }
    }
    return false;
}

bool Physics::CheckSweptAABB(HitInfo& hitInfo, const rectf& main, const vec2f& vec, const rectf& target,
                             float deltaTime)
{
    if (vec == vec2f(0.0f, 0.0f)) return false;

    rectf expended_target = rectf(target.pos - vec2f(main.w / 2.f, main.h / 2.f),
                                  target.w + main.w,
                                  target.h + main.h);

    if (RayCast(hitInfo, main.Center(), vec * deltaTime, expended_target))
    {
        if (hitInfo.CollisionTime <= 1.0f)
            return true;
    }

    return false;
}

bool Physics::RayCast(HitInfo& hitInfo, const vec2f& ray_point, const vec2f& ray_dir, const rectf& target)
{
    vec2f t_entry = (target.pos - ray_point) / ray_dir;
    vec2f t_exit = (target.pos + vec2f(target.w, target.h) - ray_point) / ray_dir;

    if (t_entry.x > t_exit.x) std::swap(t_entry.x, t_exit.x);
    if (t_entry.y > t_exit.y) std::swap(t_entry.y, t_exit.y);

    float entryTime = std::max<float>(t_entry.x, t_entry.y);
    float exitTime = std::min<float>(t_exit.x, t_exit.y);

    if (entryTime > exitTime || exitTime < 0.0f) return false;

    if (t_entry.x > t_entry.y)
        hitInfo.Normal = ray_dir.x < 0.0f ? vec2f(1.0f, 0.0f) : vec2f(-1.0f, 0.0f);
    else
        hitInfo.Normal = ray_dir.y < 0.0f ? vec2f(0.0f, 1.0f) : vec2f(0.0f, -1.0f);

    hitInfo.CollisionTime = entryTime;

    return true;
}

std::shared_ptr<RigidBody2D>& Physics::AddRigidBody(const std::shared_ptr<Entity>& owner, const vec2f& pos,
                                                    const float& w, const float& h)
{
    m_instance->m_actorBodies.emplace_back(std::make_shared<RigidBody2D>(owner, pos, w, h));
    return (*m_instance->m_actorBodies.rbegin());
}

void Physics::AddCollider(const std::shared_ptr<ICollider>& collider)
{
    m_instance->m_colliders.push_back(collider);
}

void Physics::ApplyForce(float DeltaTime)
{
    for (auto& actorBody : m_instance->m_actorBodies)
        actorBody->velocity_.y += 1500.f * DeltaTime;
}

void Physics::Update(float DeltaTime)
{
    for (auto& collider : m_instance->m_colliders)
        collider->Update(DeltaTime);

    for (auto& actorBody : m_instance->m_actorBodies)
        actorBody->Update(DeltaTime);

    RemoveColliders();
}

void Physics::PlatformResolution(float deltaTime)
{
    for (auto& actor : m_instance->m_actorBodies)
    {
        for (auto& target : m_instance->m_colliders)
        {
            HitInfo HitInfo;
            if (target->GetTag() == "tile-collision")
            {
                auto terrain = dynamic_cast<BoxCollider*>(target.get());
                if (!terrain) continue;
                if (CheckSweptAABB(HitInfo, actor->m_collider, actor->velocity_, terrain->GetCollider(), deltaTime))
                {
                    float offset = (actor->velocity_.y) / 100.f + 2.f;

                    if (HitInfo.Normal.x != 0.f)
                    {
                        actor->velocity_.x = 0;
                        if (actor->m_collider.Right() <= terrain->GetCollider().Left() && HitInfo.Normal.x > 0.f)
                            actor->m_collider.pos.x = terrain->GetCollider().Left() - actor->m_collider.w;
                        if (actor->m_collider.Left() >= terrain->GetCollider().Right() && HitInfo.Normal.x < 0.f)
                            actor->m_collider.pos.x = terrain->GetCollider().Right();
                    }
                    else if (HitInfo.Normal.y != 0.f)
                    {
                        if (actor->m_collider.Top() >= terrain->GetCollider().Bottom() && HitInfo.Normal.y > 0.f)
                        {
                            actor->m_collider.pos.y = terrain->GetCollider().Bottom();
                            if (actor->velocity_.y < 0.f)
                                actor->velocity_.y = 0.f;
                        }

                        if (actor->m_collider.Bottom() <= terrain->GetCollider().Top() && HitInfo.Normal.y < 0.f)
                        {
                            actor->m_collider.pos.y = terrain->GetCollider().Top() - actor->m_collider.h;
                            if (actor->velocity_.y > 0.f)
                                actor->velocity_.y = 0.f;
                        }
                    }
                }

                if (CheckSweptAABB(HitInfo, actor->m_collider, actor->impactVeloctity_, terrain->GetCollider(),
                                   deltaTime))
                {
                    if (actor->m_collider.Bottom() > terrain->GetCollider().Top())
                    {
                        actor->impactVeloctity_.x = 0;
                    }
                }
            }
        }
    }
}

void Physics::Render()
{
    for (auto& actorBody : m_instance->m_actorBodies)
        actorBody->Render();
}

void Physics::RemoveColliders()
{
    ProcessRemoveCollider(m_instance->m_colliders);
    ProcessRemoveCollider(m_instance->m_actorBodies);
}

void Physics::ClearData()
{
    if (!m_instance) return;
    m_instance->m_colliders.clear();
    m_instance->m_actorBodies.clear();
}

Physics::Physics()
{
}

Physics::~Physics()
{
}
