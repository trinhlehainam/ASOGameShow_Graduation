#include "Application.h"

#include <deque>

#include <DxLib.h>

#include "Controller.h"
#include "Time.h"
#include "../Math/MathHelper.h"
#include "../_debug/_DebugDispOut.h"
#include "../Systems/TextureMng.h"
#include "../Systems/AnimationMng.h"
#include "../Systems/AnimatorControllerMng.h"
#include "../Systems/Physics.h"
#include "../Systems/Renderer.h"

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

    std::unique_ptr<Controller> m_controller;
    std::deque<std::unique_ptr<IScene>> m_scenes;

    bool m_isRunning;
};

Application::Impl::Impl() :
    m_controller(std::make_unique<Controller>()), m_isRunning(true)
{
}

void Application::Impl::Update()
{
    // Update

    m_controller->Update();

    auto& time = Time::Instance();
    time.FixedFrameRate();
    
    m_scenes.back()->Update(time.DeltaTimeF());
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
    auto& time = Time::Instance();
    DrawFormatString(20, 10, GetColor(255, 255, 255), "FPS : %.f", 1.0f / time.DeltaTimeF());
    DrawFormatString(20, 30, GetColor(255, 255, 255), "Deltatime_ms : %.2f", time.DeltaTimeF() / MathHelper::kMsToSecond);
    _dbgDraw();
#endif
    ScreenFlip();
    //
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

void Application::RequestTerminate()
{
    AppInstance->m_impl->m_isRunning = false;
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

    Time::Create();
    Renderer::Create();
    Physics::Create();
    TextureMng::Create();
    AnimationMng::Create();
    AnimatorControllerMng::Create();

	// AnimationMng::LoadFromXML("Assets/Animations/animation.xml");
	AnimationMng::LoadFromXML("Assets/Animations/playerIdle.xml");
	AnimationMng::LoadFromXML("Assets/Animations/playerRun.xml");
	AnimationMng::LoadFromXML("Assets/Animations/playerAttack1.xml");
	AnimationMng::LoadFromXML("Assets/Animations/playerDeath.xml");
	AnimationMng::LoadFromXML("Assets/Animations/playerJump.xml");
    
	AnimatorControllerMng::LoadFromXML("Assets/Animators/playerAnimator.xml");
    
    m_impl->m_scenes.emplace_back(std::make_unique<TitleScene>());
    m_impl->m_scenes.back()->Init();

    return true;
}

void Application::Exit()
{
    TextureMng::Destroy();
    AnimationMng::Destroy();
    AnimatorControllerMng::Destroy();
    Physics::Destroy();
    Renderer::Destroy();
    DxLib_End();
}

void Application::Run()
{
    while (ProcessMessage() != -1 && m_impl->m_isRunning)
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