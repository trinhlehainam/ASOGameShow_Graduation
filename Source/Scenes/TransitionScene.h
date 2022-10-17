#pragma once
#include "IScene.h"

class TransitionScene :
    public IScene
{
public:
    enum class STAGE
    {
        FADE_IN,
        FADE_OUT,
        CROSS_OVER,
        DONE
    };
public:
    explicit TransitionScene(std::unique_ptr<IScene> before, std::unique_ptr<IScene> after);
    explicit TransitionScene(std::unique_ptr<IScene> before, std::unique_ptr<IScene> after, STAGE stage);
    explicit TransitionScene(std::unique_ptr<IScene> before, std::unique_ptr<IScene> after, STAGE stageBefore, STAGE stageAfter );
    ~TransitionScene();
private:
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
   void RenderCrossOver(const std::unique_ptr<IScene>& pScene);
private:
    std::unique_ptr<IScene> m_before;
    std::unique_ptr<IScene> m_after;
    STAGE m_stageBefore;
    STAGE m_stageAfter;

    RenderFunc_t m_renderBefore;
    RenderFunc_t m_renderAfter;

    float m_timer_s;
};

