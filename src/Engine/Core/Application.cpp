#include "Engine/Core/Application.h"

#include "Engine/Core/IPlatform.h"
#include "Engine/Core/Time.h"

#include "Engine/Input/IInput.h"

#include "Game/Scenes/TestScene.h"

#include <memory>

Application::Application(
    IPlatform& platform,
    IInput& input,
    IDebugText& debugText,
    ICameraBackend& cameraBackend)
    : m_platform(platform)
    , m_input(input)
    , m_debugText(debugText)
    , m_cameraBackend(cameraBackend)
{
}

Application::~Application()
{
    Shutdown();
}

int Application::Run()
{
    if (!Initialize())
    {
        return -1;
    }

    Time::Initialize();

    while (m_isRunning)
    {
        if (!m_platform.ProcessEvents())
        {
            m_isRunning = false;
            break;
        }

        Time::Update();

        Update();
        Render();
    }

    Shutdown();

    return 0;
}

bool Application::Initialize()
{
    if (m_isInitialized)
    {
        return true;
    }

    if (!m_platform.Initialize())
    {
        return false;
    }

    // EN: Application acts as the composition root for the initial scene.
    //     It supplies the scene's required dependencies without making
    //     SceneManager responsible for constructing gameplay objects.
    //
    // JP: Application は最初の Scene を構築する Composition Root として働く。
    //     必要な依存関係をここから渡し、SceneManager に Gameplay オブジェクトの
    //     生成責任を持たせない。
    m_sceneManager.RequestChange(
        std::make_unique<TestScene>(
            m_input,
            m_debugText,
            m_cameraBackend));


    m_isInitialized = true;
    m_isRunning = true;

    return true;
}

void Application::Shutdown()
{
    if (!m_isInitialized)
    {
        return;
    }

    // EN: Destroy scenes while platform services are still alive.
    //     Scene shutdown may eventually release rendering, audio, or other
    //     platform-backed resources.
    //
    // JP: Platform がまだ有効な間に Scene を終了・破棄する。
    //     将来的に Scene が描画・音声などのプラットフォーム依存リソースを
    //     解放する可能性があるため、この順序を保証する。
    m_sceneManager.Shutdown();

    m_platform.Shutdown();

    m_isInitialized = false;
    m_isRunning = false;
}

void Application::Update()
{
    m_input.Update();


    // EN: Scene transitions are committed before scene logic executes,
    //     ensuring that destruction never occurs inside a scene callback.
    //
    // JP: Scene の処理を実行する前に予約された切り替えを確定し、
    //     Scene のコールバック実行中に破棄が発生しないようにする。
    m_sceneManager.ApplyPendingChange();

    m_sceneManager.Update(Time::DeltaTime());
}

void Application::Render()
{
    m_platform.BeginFrame();

    // test
    m_sceneManager.Render();

    m_platform.EndFrame();
}