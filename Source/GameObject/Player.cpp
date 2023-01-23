#include "Player.h"

#include "../_debug/_DebugDispOut.h"

#include "Entity.h"

#include "../Systems/TextureMng.h"
#include "../Systems/Physics.h"
#include "../Systems/EntityMng.h"

#include "../Input/KeyboardInput.h"
#include "../Input/JoypadXInput.h"
#include "../Input/InputCommand.h"

#include "../Component/TransformComponent.h"
#include "../Component/Animation/Animator.h"
#include "../Component/Collider/RigidBody2D.h"

namespace
{
    int color = 0;
    vec2f dir;
}

Player::Player(const std::shared_ptr<EntityMng>& entityMng): m_entity(std::make_shared<Entity>(entityMng))
{
    entityMng->AddEntity(m_entity);
    isJumpRequested = false;
}

Player::~Player()
{
}

void Player::Init(INPUT_DEVICE deviceId)
{
    TextureMng::AddImage("knight", "Assets/Textures/knight 1 axe.png");

    switch (deviceId)
    {
    case INPUT_DEVICE::KEYBOARD:
        m_input = std::make_shared<KeyboardInput>();
        break;
    case INPUT_DEVICE::JOYPAD:
        m_input = std::make_shared<JoypadXInput>();
        break;
    }

    m_inputCommand = std::make_shared<InputCommand>(m_input);
    m_inputCommand->LoadPatternFromXML("Assets/InputPatterns/inputpattern.xml");

    m_entity->SetTag("knight");
    m_entity->AddComponent<TransformComponent>(m_entity);
    auto transform = m_entity->GetComponent<TransformComponent>();
    transform->Pos = vec2f{400.0f, 50.0f};
    transform->Scale = vec2f{1.5f, 1.5f};

    m_entity->AddComponent<Animator>(m_entity);
    auto animator = m_entity->GetComponent<Animator>();
    animator->AddAnimatorController("playerAnimator");

    const auto& rigidBody = Physics::AddRigidBody(
        m_entity,
        transform->Pos,
        32.f * transform->Scale.x,
        32.f * transform->Scale.y);
    m_body = rigidBody;
    rigidBody->SetMaxVelocity(700.f, 1200.f);
}

void Player::Update(float deltaTime_s)
{
    m_input->Update();
    m_inputCommand->Update();
    auto speed = vec2f{0.0f, m_body->velocity_.y};

    const auto& transform = m_entity->GetComponent<TransformComponent>();
    auto animator = m_entity->GetComponent<Animator>();

    if (m_input->IsPressed(INPUT_ID::LEFT))
    {
        speed.x = -200.0f;
        animator->SetFlip(true);
    }
    else if (m_input->IsPressed(INPUT_ID::RIGHT))
    {
        speed.x = 200.0f;
        animator->SetFlip(false);
    }

    if (m_input->IsJustPressed(INPUT_ID::BTN2))
        isJumpRequested = true;


    animator->SetFloat("speed", m_body->velocity_.x);

    animator->SetBool("isAttack", m_input->IsPressed(INPUT_ID::BTN1));

    animator->SetBool("isDeath", m_inputCommand->IsMatch("combo-2", 0.5f));

    bool isGrounded = Physics::RayCast(transform->Pos, vec2f(0.f, m_body->m_offset.y), m_body->m_offset.y + 3.f);

    animator->SetBool("isGrounded", isGrounded);

    if (isJumpRequested && isGrounded)
    {
        speed.y = -500.f;
    }
    
    isJumpRequested = false;
    
    m_body->velocity_ = speed;
}

void Player::Render()
{
#ifdef _DEBUG
    const auto& transform = m_entity->GetComponent<TransformComponent>();
    vec2f origin{transform->Pos};
    vec2f end = origin + vec2f(0.f, m_body->m_offset.y + 3.f);
    DxLib::DrawLineAA(origin.x, origin.y, end.x, end.y, 0x00ff00, 2.0f);
#endif
}

void Player::Destroy()
{
    m_entity->Destroy();
}

std::shared_ptr<Entity> Player::GetEntity() const
{
    return m_entity;
}
