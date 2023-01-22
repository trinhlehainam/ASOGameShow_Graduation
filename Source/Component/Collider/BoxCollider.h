#pragma once
#include "ICollider.h"
#include "../../Math/rect.h"


class BoxCollider :
    public ICollider
{
public:
    explicit BoxCollider(const std::shared_ptr<Entity>& owner);
    explicit BoxCollider(const std::shared_ptr<Entity>& owner, const rectf& boxInfo);
    ~BoxCollider();

    void SetOrigin(const vec2f& origin);
    void SetSize(const vec2f& size);

    vec2f GetOrigin() const;
    vec2f GetSize() const;
    rectf GetCollider() const;
public:
    void Init() override final;
    void Update(float deltaTime_s) override final;
    void Render() override final;
    COLLIDER_TYPE ColliderType() override final;
private:
    rectf m_collider;
};

