#include "TransitionScene.h"

#include <DxLib.h>

namespace
{
    constexpr float kTransitionTime = 3.0f;
}

TransitionScene::TransitionScene(std::unique_ptr<IScene> before, std::unique_ptr<IScene> after):
    m_before(std::move(before)), m_after(std::move(after)),
    m_stageBefore(STAGE::CROSS_OVER), m_stageAfter(STAGE::DONE),
    m_timer_s(kTransitionTime),
    m_renderBefore(&TransitionScene::RenderCrossOver), m_renderAfter(&TransitionScene::RenderSleep)
{
    m_after->Init();
}

TransitionScene::TransitionScene(std::unique_ptr<IScene> before, std::unique_ptr<IScene> after, STAGE stage):
    m_before(std::move(before)), m_after(std::move(after)),
    m_stageBefore(stage), m_stageAfter(stage),
    m_timer_s(kTransitionTime),
    m_renderAfter(&TransitionScene::RenderSleep)
{
    SetUpStageBefore();
     m_after->Init();
}

TransitionScene::TransitionScene(std::unique_ptr<IScene> before, std::unique_ptr<IScene> after, STAGE stageBefore, STAGE stageAfter):
    m_before(std::move(before)), m_after(std::move(after)),
    m_stageBefore(stageBefore), m_stageAfter(stageAfter),
    m_timer_s(kTransitionTime),
    m_renderAfter(&TransitionScene::RenderSleep)
{
    SetUpStageBefore();
    m_after->Init();
}

TransitionScene::~TransitionScene() {}

bool TransitionScene::Init()
{
    return true;
}

void TransitionScene::Update(float deltaTime_s)
{
    m_timer_s -= deltaTime_s;
    ChangeStage();
}

void TransitionScene::RenderToOwnScreen()
{
    DxLib::SetDrawScreen(m_screenID);
    DxLib::ClearDrawScreen();
    (this->*m_renderBefore)(m_before);
    (this->*m_renderAfter)(m_after);
}

std::unique_ptr<IScene> TransitionScene::ChangeScene(std::unique_ptr<IScene> scene)
{
    EnableChangeScene = false;
    return std::move(m_after);
}

SCENE_ID TransitionScene::GetSceneID()
{
    return SCENE_ID::TRANSITION;
}

void TransitionScene::SetUpStageBefore()
{
    switch (m_stageBefore)
    {
    case STAGE::CROSS_OVER:
        m_stageAfter = STAGE::DONE;
        m_renderBefore = &TransitionScene::RenderCrossOver;
        break;
    case STAGE::FADE_IN:
        m_renderBefore = &TransitionScene::RenderFadeIn;
        break;
    case STAGE::FADE_OUT:
        m_renderBefore = &TransitionScene::RenderFadeOut;
        break;
    case STAGE::DONE:
        m_renderBefore = &TransitionScene::RenderSleep;
        break;
    default:
        break;
    }
}

void TransitionScene::SetUpStageAfter()
{
    switch (m_stageAfter)
    {
    case STAGE::FADE_IN:
        m_renderAfter = &TransitionScene::RenderFadeIn;
        break;
    case STAGE::FADE_OUT:
        m_renderAfter = &TransitionScene::RenderFadeOut;
        break;
    case STAGE::DONE:
        EnableChangeScene = true;
        m_renderAfter = &TransitionScene::RenderSleep;
        break;
    default:
        break;
    }
}

void TransitionScene::ChangeStage()
{
    if (m_timer_s < 0.0f)
    {
        m_timer_s = kTransitionTime;
        m_stageBefore = STAGE::DONE;
        SetUpStageBefore();
        SetUpStageAfter();
        m_stageAfter = STAGE::DONE;
    }
}

void TransitionScene::RenderSleep(const std::unique_ptr<IScene>& pScene) {}

void TransitionScene::RenderFadeOut(const std::unique_ptr<IScene>& pScene)
{
    int alpha = static_cast<int>((m_timer_s * 255) / kTransitionTime);
    DxLib::SetDrawBlendMode(DX_BLENDMODE_ADD, alpha);
    pScene->Render();
    DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
}

void TransitionScene::RenderFadeIn(const std::unique_ptr<IScene>& pScene)
{
    int alpha = static_cast<int>((m_timer_s * 255) / kTransitionTime);
    DxLib::SetDrawBlendMode(DX_BLENDMODE_ADD, 255 - alpha);
    pScene->Render();
    DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
}

void TransitionScene::RenderCrossOver(const std::unique_ptr<IScene>& pScene)
{
    int alpha = static_cast<int>((m_timer_s * 255) / kTransitionTime);
    DxLib::SetDrawBlendMode(DX_BLENDMODE_ADD, alpha);
    m_before->Render();
    DxLib::SetDrawBlendMode(DX_BLENDMODE_ADD, 255 - alpha);
    m_after->Render();
    DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
}
