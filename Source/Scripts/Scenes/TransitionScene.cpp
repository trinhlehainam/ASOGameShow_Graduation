#include "TransitionScene.h"

#include <DxLib.h>

TransitionScene::TransitionScene(std::unique_ptr<IScene> before, std::unique_ptr<IScene> after, TRANSITION transition,
                                 float transitionTime_s):
    m_sceneBefore(std::move(before)), m_sceneAfter(std::move(after)),
    m_renderBefore(&TransitionScene::RenderSleep), m_renderAfter(&TransitionScene::RenderSleep),
    m_transiftionTime_s(transitionTime_s),
    m_timer_s(transitionTime_s)
{
    switch (transition)
    {
    case TRANSITION::FADE_OUT_FADE_IN:
        m_stageBefore = STAGE::FADE_OUT;
        m_stageAfter = STAGE::FADE_IN;
        break;
    case TRANSITION::CROSS_OVER:
        m_stageBefore = STAGE::CROSS_OVER;
        m_stageAfter = STAGE::CROSS_OVER;
        break;
    case TRANSITION::NONE:
        m_stageBefore = STAGE::DONE;
        m_stageAfter = STAGE::DONE;
        break;
    }
    
    SetUpStageBefore();
    SetUpStageAfter();
    m_sceneAfter->Init();
}

TransitionScene::~TransitionScene()
{
}

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

    (this->*m_renderBefore)(m_sceneBefore);
    (this->*m_renderAfter)(m_sceneAfter);

    RenderEnd();
}

std::unique_ptr<IScene> TransitionScene::ChangeScene(std::unique_ptr<IScene> scene)
{
    IsChangeSceneRequested = false;
    return std::move(m_sceneAfter);
}

SCENE_ID TransitionScene::GetSceneID()
{
    return SCENE_ID::TRANSITION;
}

void TransitionScene::SetUpStageBefore()
{
    switch (m_stageBefore)
    {
    case STAGE::FADE_IN:
        m_renderBefore = &TransitionScene::RenderFadeIn;
        break;
    case STAGE::CROSS_OVER:
    case STAGE::FADE_OUT:
        m_renderBefore = &TransitionScene::RenderFadeOut;
        break;
    case STAGE::DONE:
        m_renderBefore = &TransitionScene::RenderSleep;
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
    case STAGE::CROSS_OVER:
        m_renderAfter = &TransitionScene::RenderFadeIn;
        m_stageAfter = STAGE::DONE;
        break;
    case STAGE::DONE:
        IsChangeSceneRequested = true;
        m_renderAfter = &TransitionScene::RenderSleep;
        break;
    }
}

void TransitionScene::ChangeStage()
{
    if (m_timer_s < 0.0f)
    {
        m_timer_s = m_transiftionTime_s;
        m_stageBefore = STAGE::DONE;
        SetUpStageBefore();
        SetUpStageAfter();
        m_stageAfter = STAGE::DONE;
    }
}

void TransitionScene::RenderSleep(const std::unique_ptr<IScene>& pScene)
{
}

void TransitionScene::RenderFadeOut(const std::unique_ptr<IScene>& pScene)
{
    int alpha = static_cast<int>((m_timer_s * 255) / m_transiftionTime_s);
    DxLib::SetDrawBlendMode(DX_BLENDMODE_ADD, alpha);
    pScene->Render();
}

void TransitionScene::RenderFadeIn(const std::unique_ptr<IScene>& pScene)
{
    int alpha = static_cast<int>((m_timer_s * 255) / m_transiftionTime_s);
    DxLib::SetDrawBlendMode(DX_BLENDMODE_ADD, 255 - alpha);
    pScene->Render();
}

void TransitionScene::RenderEnd()
{
    DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
}
