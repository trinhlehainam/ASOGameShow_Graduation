#include "MenuScene.h"

#include <assert.h>
#include <DxLib.h>

#include "../../Common.h"
#include "../../Input/KeyboardInput.h"
#include "TitleScene.h"
#include "../../Math/rect.h"
#include "../../Systems/Application.h"
#include "../../Systems/Controller.h"
// #include "../Scene/KeyConfigScene.h"

namespace
{
	constexpr float appear_time = 0.5f;
	constexpr float disappear_time = 0.5f;
	float timer_ = 0;
	int indicatorTexture_ = -1;

	constexpr int menu_y_interval = 48;		// Space between each item
	const char* pauseTitleText = "Pause Scene";

	constexpr int selected_item_offset = 20;;
	int currentItemNo_ = 0;
	int pauseTextLength = 0;
	int itemSize = 0;

	constexpr float box_width = 300.0f;
	constexpr float box_height = 300.0f;
	const rectf menuBox(vec2f((kScreenWidth - box_width) / 2.0f, (kScreenHeight - box_height) / 2.0f), box_width, box_height);
	constexpr int box_offset_x = 50;
	constexpr int box_offset_y = 50;
}

MenuScene::MenuScene()
{
	drawFunc_ = &MenuScene::AppearDraw;
	updateFunc_ = &MenuScene::AppearUpdate;

	if (indicatorTexture_ == -1)
	{
		indicatorTexture_ = DxLib::LoadGraph("Assets/Textures/UI/indicator.png");
	}
	
	timer_ = 0;

	pauseTextLength = DxLib::GetStringLength(pauseTitleText);

	int x = menuBox.Left() + 100;
	int y = menuBox.Top() + 100;
	menuItems_.emplace_back("ゲームに戻る", vec2f(x, y),
		[this]()
		{
			CloseScene();
		});
	y += menu_y_interval;
	menuItems_.emplace_back("タイトルに戻る", vec2f(x, y),
		[this]()
		{
			Application::ResetScene(std::make_unique<TitleScene>());
		});
	y += menu_y_interval;
	menuItems_.emplace_back("キーコンフィグ", vec2f(x, y),
		[this]()
		{
			// sceneMng_.PushScene(std::move(active_scene(new KeyConfigScene(sceneMng_, sceneInput_, playerInput_))));
		});
	y += menu_y_interval;
	menuItems_.emplace_back("ゲームを終了", vec2f(x, y),
		[this]()
		{
			Application::RequestTerminate();
		});

	itemSize = static_cast<int>(menuItems_.size());
	currentItemNo_ = 0;

	SetCurrentItem();
}

MenuScene::~MenuScene()
{
	DxLib::DeleteGraph(indicatorTexture_);
	indicatorTexture_ = -1;
}

bool MenuScene::Init()
{
	return true;
}

void MenuScene::Update(float deltaTime)
{
	(this->*updateFunc_)(deltaTime);
}

void MenuScene::RenderToOwnScreen()
{
    DxLib::SetDrawScreen(m_screenID);
    DxLib::ClearDrawScreen();
	
	(this->*drawFunc_)();
}

void MenuScene::Render()
{
	DxLib::DrawGraphF(m_screenOffsetX, m_screenOffsetY, m_screenID, 1);
}

std::unique_ptr<IScene> MenuScene::ChangeScene(std::unique_ptr<IScene> scene)
{
    IsChangeSceneRequested = false;
	return scene;
}

SCENE_ID MenuScene::GetSceneID()
{
	return SCENE_ID::MENU;
}

void MenuScene::NormalDraw()
{

	// Draw Menu Box
	DxLib::SetDrawBlendMode(DX_BLENDMODE_MULA, 128);
	DxLib::DrawBox(menuBox.Left(),
		menuBox.Top(),
		menuBox.Right(),
		menuBox.Bottom(),
		0x000000, true);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DxLib::DrawBoxAA(menuBox.Left(),
		menuBox.Top(),
		menuBox.Right(),
		menuBox.Bottom(),
		0xffffff, false, 2.0f);

	// Draw Pause Title Text
	DxLib::DrawString(menuBox.Center().x - 50, menuBox.Top() + 20, pauseTitleText, 0xffffff);

	// Draw Menu Item
	for (auto& itr : menuItems_)
	{
		int offset = 0;
		int color = 0xffffff;
		if (itr.isActive)
		{
			color = 0xaa0000;
			offset = selected_item_offset;
			DrawGraph(itr.pos.x - 50, itr.pos.y, indicatorTexture_, true);
		}

		DxLib::DrawString(itr.pos.x + offset, itr.pos.y, itr.menuText.c_str(), color);
	}

}

void MenuScene::AppearDraw()
{
	int expend = (menuBox.h / 2) / appear_time;
	DxLib::SetDrawBlendMode(DX_BLENDMODE_MULA, 128);
	DxLib::DrawBox(menuBox.Left(),
		menuBox.Center().y - (expend * timer_),
		menuBox.Right(),
		menuBox.Center().y + (expend * timer_),
		0x000000, true);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DxLib::DrawBoxAA(menuBox.Left(),
		menuBox.Center().y - (expend * timer_),
		menuBox.Right(),
		menuBox.Center().y + (expend * timer_),
		0xffffff, false, 2.0f);
}

void MenuScene::DisappearDraw()
{
	float expend = (menuBox.h / 2) / appear_time;
	DxLib::SetDrawBlendMode(DX_BLENDMODE_MULA, 128);
	DxLib::DrawBox(menuBox.Left(),
		menuBox.Top() + (expend * timer_),
		menuBox.Right(),
		menuBox.Bottom() - (expend * timer_),
		0x000000, true);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DxLib::DrawBoxAA(menuBox.Left(),
		menuBox.Top() + (expend * timer_),
		menuBox.Right(),
		menuBox.Bottom() - (expend * timer_),
		0xffffff, false, 2.0f);
}

void MenuScene::NormalUpdate(const float& deltaTime)
{
	const auto Controller = Application::GetController();
	assert(Controller);
	
	// Return to previous scene
	if (Controller->IsJustPressed(INPUT_ID::BTN4))
	{
		CloseScene();
	}

	// Move between items
	if (Controller->IsJustPressed(INPUT_ID::UP))
	{
		currentItemNo_ = (currentItemNo_ - 1 + itemSize) % itemSize;
	}
	
	if (Controller->IsJustPressed(INPUT_ID::DOWN))
	{
		currentItemNo_ = (currentItemNo_ + 1) % itemSize;
	}

	SetCurrentItem();

	// Select Menu Item
	if (Controller->IsJustPressed(INPUT_ID::SELECT))
	{
		menuItems_[currentItemNo_].func();
	}
}

void MenuScene::SetCurrentItem()
{
	for (auto& itr : menuItems_)
	{
		itr.isActive = false;
	}
	menuItems_[currentItemNo_].isActive = true;
}

void MenuScene::CloseScene()
{
	updateFunc_ = &MenuScene::DisappearUpdate;
	drawFunc_ = &MenuScene::DisappearDraw;
	timer_ = 0;
}

void MenuScene::AppearUpdate(const float& deltaTime)
{
	if (timer_ >= appear_time)
	{
		updateFunc_ = &MenuScene::NormalUpdate;
		drawFunc_ = &MenuScene::NormalDraw;
		timer_ = 0;
	}
	timer_ += deltaTime;
}

void MenuScene::DisappearUpdate(const float& deltaTime)
{
	if (timer_ >= appear_time)
	{
		Application::PopScene();
	}
	timer_ += deltaTime;
}
