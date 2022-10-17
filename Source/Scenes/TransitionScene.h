#pragma once
#include "IScene.h"

class TransitionScene :
    public IScene
{
public:
    enum class TRANSITION
    {
        FADE_OUT_FADE_IN,
        CROSS_OVER,
        NONE
    };

public:
    explicit TransitionScene(std::unique_ptr<IScene> before, std::unique_ptr<IScene> after, TRANSITION transition, float transitionTime_s);
    ~TransitionScene();
private:
    enum class STAGE
    {
        FADE_IN,
        FADE_OUT,
        CROSS_OVER,
        DONE
    };
    
    bool Init() override;
    void Update(float deltaTime_s) override;
    void RenderToOwnScreen() override;
    std::unique_ptr<IScene> ChangeScene(std::unique_ptr<IScene> scene) override;
    SCENE_ID GetSceneID() override;

private:
    using RenderFunc_t = void (TransitionScene::*)(const std::unique_ptr<IScene>&);

    void SetUpStageBefore();
    void SetUpStageAfter();
    void ChangeStage();

    void RenderSleep(const std::unique_ptr<IScene>& pScene);
    void RenderFadeOut(const std::unique_ptr<IScene>& pScene);
    void RenderFadeIn(const std::unique_ptr<IScene>& pScene);

    void RenderEnd();
private:
    std::unique_ptr<IScene> m_sceneBefore;
    std::unique_ptr<IScene> m_sceneAfter;
    STAGE m_stageBefore;
    STAGE m_stageAfter;

    RenderFunc_t m_renderBefore;
    RenderFunc_t m_renderAfter;

    const float m_transiftionTime_s;
    float m_timer_s;
};
