#include "Application.h"

#include <chrono>
#include <deque>

#include <DxLib.h>

#include "Controller.h"
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

    std::unique_ptr<Controller> m_controller;
    std::deque<std::unique_ptr<IScene>> m_scenes;
    std::chrono::steady_clock::time_point m_lastTime;
    float m_deltaTime_s;
};

Application::Impl::Impl() :
    m_controller(std::make_unique<Controller>()),
    m_lastTime(std::chrono::high_resolution_clock::now()),
    m_deltaTime_s(0.0f)
{
}

void Application::Impl::Update()
{
    // Update
    m_controller->Update();
    
    m_deltaTime_s = GetDeltaTime_s();
    m_lastTime = std::chrono::high_resolution_clock::now();
    m_scenes.back()->Update(m_deltaTime_s);
    //

    // Change/Move scene
    if (m_scenes.back()->IsChangeSceneRequested)
        m_scenes.back() = m_scenes.back()->ChangeScene(std::move(m_scenes.back()));
    //
}

void Application::Impl::Render()
{
#ifdef _DEBUG
    _dbgStartDraw();
#endif
    for (const auto& scene : m_scenes)
        scene->RenderToOwnScreen();
    // m_scenes.back()->RenderToOwnScreen();

    // Render to screen back
    SetDrawScreen(DX_SCREEN_BACK);
    ClearDrawScreen();
    // m_scenes.back()->Render();
    for (const auto& scene : m_scenes)
        scene->Render();
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
        std::chrono::high_resolution_clock::now() - m_lastTime).count();
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
    AppInstance->m_impl->m_scenes.back() = std::move(scene);
}

void Application::ClearScene()
{
    if (!AppInstance) return;
    
    AppInstance->m_impl->m_scenes.clear();
}

void Application::ResetScene(std::unique_ptr<IScene> scene)
{
    if (!AppInstance) return;
    
    AppInstance->m_impl->m_scenes.clear();
    AppInstance->m_impl->m_scenes.emplace_back(std::move(scene));
}

void Application::PushScene(std::unique_ptr<IScene> scene)
{
    if (!AppInstance) return;
    AppInstance->m_impl->m_scenes.emplace_back(std::move(scene));
}

void Application::PopScene()
{
    if (!AppInstance) return;
    AppInstance->m_impl->m_scenes.pop_back();
}

Controller* Application::GetController()
{
    return AppInstance->m_impl->m_controller.get();
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

	AnimationMng::LoadFromXML("Assets/Animations/animation.xml");
	AnimatorControllerMng::LoadFromXML("Assets/Animators/playerAnimator.xml");
    
    m_impl->m_scenes.emplace_back(std::make_unique<TitleScene>());
    m_impl->m_scenes.back()->Init();

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
    while (ProcessMessage() != -1)
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