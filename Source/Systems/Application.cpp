#include "Application.h"

#include <chrono>
#include <deque>

#include <DxLib.h>

#include "../_debug/_DebugDispOut.h"
#include "../Systems/TextureMng.h"
#include "../Systems/AnimationMng.h"
#include "../Systems/AnimatorControllerMng.h"
#include "../Systems/Physics.h"
#include "../Systems/Renderer.h"
#include "../Math/MathHelper.h"

#include "../Common.h"
#include "../Scenes/TitleScene.h"
#include "../Scenes/GameScene.h"

namespace
{
    Application* AppInstance = nullptr;
}

#pragma region Pimpl
class Application::Impl
{
public:
    Impl();
    ~Impl() = default;

    void Update();
    void Render();

    float GetDeltaTime_s();

    std::deque<std::unique_ptr<IScene>> scenes;
    std::chrono::steady_clock::time_point lastTime;
    float m_deltaTime_s;
};

Application::Impl::Impl() : lastTime(std::chrono::high_resolution_clock::now()), m_deltaTime_s(0.0f)
{
}

void Application::Impl::Update()
{
    // Update
    m_deltaTime_s = GetDeltaTime_s();
    lastTime = std::chrono::high_resolution_clock::now();
    scenes.back()->Update(m_deltaTime_s);
    //

    // Change/Move scene
    if (scenes.back()->IsChangeSceneRequested)
        scenes.back() = scenes.back()->ChangeScene(std::move(scenes.back()));
    //
}

void Application::Impl::Render()
{
#ifdef _DEBUG
    _dbgStartDraw();
#endif
    scenes.back()->RenderToOwnScreen();

    // Render to screen back
    SetDrawScreen(DX_SCREEN_BACK);
    ClearDrawScreen();
    scenes.back()->Render();
    // Show FPS
#ifdef _DEBUG
    DrawFormatString(20, 10, GetColor(255, 255, 255), "FPS : %.f", 1.0f / m_deltaTime_s);
    DrawFormatString(20, 30, GetColor(255, 255, 255), "Deltatime_ms : %.2f", m_deltaTime_s / MathHelper::kMsToSecond);
    _dbgDraw();
#endif
    ScreenFlip();
    //
}

float Application::Impl::GetDeltaTime_s()
{
    return std::chrono::duration<float, std::chrono::seconds::period>(
        std::chrono::high_resolution_clock::now() - lastTime).count();
}
#pragma endregion

Application& Application::Instance()
{
    if (!AppInstance)
        AppInstance = new Application();
    return *AppInstance;
}

bool Application::Initialize()
{
    if (!AppInstance)
        AppInstance = new Application();

    return AppInstance->Init();
}

void Application::Execute()
{
    if (!AppInstance)
    {
        AppInstance = new Application();
        AppInstance->Init();
    }

    AppInstance->Run();
}

void Application::Terminate()
{
    if (!AppInstance) return;

    AppInstance->Exit();
    
    delete AppInstance;
    AppInstance = nullptr;
}

void Application::ChangeScene(std::unique_ptr<IScene> scene)
{
    if (!AppInstance) return;
    AppInstance->m_impl->scenes.back() = std::move(scene);
}

void Application::ClearScene()
{
    if (!AppInstance) return;
    
    AppInstance->m_impl->scenes.clear();
}

void Application::ResetScene(std::unique_ptr<IScene> scene)
{
    if (!AppInstance) return;
    
    AppInstance->m_impl->scenes.clear();
    AppInstance->m_impl->scenes.emplace_back(std::move(scene));
}

void Application::PushScene(std::unique_ptr<IScene> scene)
{
    if (!AppInstance) return;
    AppInstance->m_impl->scenes.emplace_back(std::move(scene));
}

void Application::PopScene()
{
    if (!AppInstance) return;
    AppInstance->m_impl->scenes.pop_back();
}

Application::Application(): m_impl(std::make_unique<Impl>())
{
}

Application::~Application()
{
}

bool Application::Init()
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
    TextureMng::Create();
    AnimationMng::Create();
    AnimatorControllerMng::Create();

    m_impl->scenes.emplace_back(std::make_unique<TitleScene>());
    m_impl->scenes.back()->Init();

    return true;
}

void Application::Exit()
{
    m_impl.release();
    TextureMng::Destroy();
    AnimationMng::Destroy();
    AnimatorControllerMng::Destroy();
    Physics::Destroy();
    Renderer::Destroy();
    DxLib_End();
}

void Application::Run()
{
    while (ProcessMessage() != -1 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
    {
        m_impl->Update();
        m_impl->Render();
    }
}

// Avoid copy and assign
Application::Application(const Application&)
{
}

Application::Application(Application&&) noexcept
{
}

void Application::operator=(const Application&)
{
}

void Application::operator=(Application&&) noexcept
{
}

//