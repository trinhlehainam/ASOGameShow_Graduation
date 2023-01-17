#include "TitleScene.h"

#include <cassert>
#include <DxLib.h>

#include "../Systems/TextureMng.h"
#include "../Systems/Application.h"
#include "../Input/InputID.h"
#include "../Systems/Controller.h"

#include "../Common.h"
#include "TransitionScene.h"
#include "GameScene.h"


TitleScene::TitleScene()
{
}

TitleScene::~TitleScene()
{
    TextureMng::DeleteImage("title");
}

bool TitleScene::Init()
{
    TextureMng::AddImage("Assets/Textures/PRESS ENTER KEY.png", "Press enter");

    return true;
}

void TitleScene::Update(float deltaTime_s)
{
    const auto Controller = Application::GetController();
	assert(Controller);
	
	// Return to previous scene
	if (Controller->IsJustPressed(INPUT_ID::SELECT))
        IsChangeSceneRequested = true;
}

void TitleScene::RenderToOwnScreen()
{
    DxLib::SetDrawScreen(m_screenID);
    DxLib::ClearDrawScreen();

    auto imageId = TextureMng::GetID("Press enter");
    float x, y;
    DxLib::GetGraphSizeF(imageId, &x, &y);
    float posX = (kScreenWidth - x) / 2.f;
    float posY = (kScreenHeight - y) / 2.f;
    DxLib::DrawGraphF(posX, posY, imageId, 0);
}

std::unique_ptr<IScene> TitleScene::ChangeScene(std::unique_ptr<IScene> scene)
{
    IsChangeSceneRequested = false;
    scene = std::make_unique<TransitionScene>(std::move(scene), std::make_unique<GameScene>(),
                                              TransitionScene::TRANSITION::CROSS_OVER, 3.f);
    scene->Init();
    return scene;
}

SCENE_ID TitleScene::GetSceneID()
{
    return SCENE_ID::TITLE;
}
