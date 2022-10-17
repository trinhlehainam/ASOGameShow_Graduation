#include "SceneMng.h"

#include <chrono>

#include <DxLib.h>

#include "../_debug/_DebugDispOut.h"
#include "../Math/MathHelper.h"
#include "../Systems/ImageMng.h"
#include "../Systems/AnimationMng.h"
#include "../Systems/AnimatorControllerMng.h"
#include "../Systems/Physics.h"
#include "../Systems/Renderer.h"

#include "../Scenes/TitleScene.h"
#include "../Scenes/GameScene.h"

namespace
{
	constexpr int kScreenWidth = 1024;
	constexpr int kScreenHeight = 768;
}

#pragma region Pimpl
class SceneMng::Impl
{
public:
	Impl();
	~Impl() = default;

	void Update();
	void Render();

	float GetDeltaTime_s();

	std::unique_ptr<IScene> scene;
	std::chrono::steady_clock::time_point lastTime;
	float m_deltaTime_s;
};

SceneMng::Impl::Impl() : lastTime(std::chrono::high_resolution_clock::now()), m_deltaTime_s(0.0f){}

void SceneMng::Impl::Update()
{
	// Update
	m_deltaTime_s = GetDeltaTime_s();
	lastTime = std::chrono::high_resolution_clock::now();
	scene->Update(m_deltaTime_s);
	//

	// Change/Move scene
	if (scene->EnableChangeScene)
		scene = std::move(scene->ChangeScene(std::move(scene)));
	//
}

void SceneMng::Impl::Render()
{
	_dbgStartDraw();
	scene->RenderToOwnScreen();

	// Render to screen back
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();
	scene->Render();
	// Show FPS
#ifdef _DEBUG || DEBUG
	DrawFormatString(20, 10, GetColor(255, 255, 255), "FPS : %.f", 1.0f / m_deltaTime_s);
	DrawFormatString(20, 30, GetColor(255, 255, 255), "Deltatime_ms : %.2f", m_deltaTime_s / MathHelper::kMsToSecond);
	_dbgDraw();
#endif
	ScreenFlip();
	//
}

float SceneMng::Impl::GetDeltaTime_s()
{
	return std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - lastTime).count();
}
#pragma endregion

SceneMng& SceneMng::Instance()
{
    static SceneMng sceneMng;
    return sceneMng;
}

SceneMng::SceneMng():m_impl(std::make_unique<Impl>()) {}

SceneMng::~SceneMng() {}

bool SceneMng::Init()
{
    SetMainWindowText("1916021_TRINH LE HAI NAM");
    ChangeWindowMode(true);
    SetGraphMode(kScreenWidth, kScreenHeight, 32);
    SetDrawScreen(DX_SCREEN_BACK);

    if (DxLib_Init() == -1)
        return false;

    _dbgSetup(kScreenWidth, kScreenHeight, 255);

	Renderer::Create();
	Physics::Create();
	ImageMng::Create();
	AnimationMng::Create();
	AnimatorControllerMng::Create();

	m_impl->scene = std::make_unique<TitleScene>();
	m_impl->scene->Init();

    return true;
}

void SceneMng::Exit()
{
	m_impl.release();
	ImageMng::Destroy();
	AnimationMng::Destroy();
	AnimatorControllerMng::Destroy();
	Physics::Destroy();
	Renderer::Destroy();
    DxLib_End();
}

void SceneMng::Run()
{
	while (ProcessMessage() != -1 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
	{
		m_impl->Update();
		m_impl->Render();
	}
}

// Avoid copy and assign
SceneMng::SceneMng(const SceneMng&) {}
SceneMng::SceneMng(SceneMng&&) noexcept {}
void SceneMng::operator=(const SceneMng&) {}
void SceneMng::operator=(SceneMng&&) noexcept {}
//

