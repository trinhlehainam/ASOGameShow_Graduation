#pragma once
#include "IScene.h"

class TitleScene :
    public IScene
{
public:
    TitleScene();
    ~TitleScene();
private:
    bool Init() override;
    void Update(float deltaTime_s) override;
    void RenderToOwnScreen() override;
    std::unique_ptr<IScene> ChangeScene(std::unique_ptr<IScene> scene) override;
    SCENE_ID GetSceneID() override;
};

