#pragma once
#include <string>

#include "../IComponent.h"
#include "../../Math/vec2.h"

enum class COLLIDER_TYPE
{
    BOX,
    CIRCLE,
    TRIANGLE,
    CAPSULE,
    RIGID_BODY_2D
};

// Base class for all coliders
class ICollider :
    public IComponent
{
public:
    std::string m_tag;
public:
    ICollider(const std::shared_ptr<Entity>& owner);
    virtual ~ICollider();

    void SetEnable(bool enableFlag);
    void SetOffset(float offsetX, float offsetY);
    void SetOffset(const vec2f& offset);
    std::string GetTag() const;

    bool IsEnable() const;
    vec2f GetOffset() const;
public:
    virtual void Init() = 0;
    virtual void Update(float deltaTime_s) = 0;
    virtual void Render() = 0;
    virtual COLLIDER_TYPE ColliderType() = 0;
protected:
    bool m_isEnable;
    vec2f m_offset;
};

