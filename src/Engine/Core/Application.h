#pragma once

#include "Engine/Scene/SceneManager.h"
#include "Engine/Resources/ResourceSystem.h"

// forward declaration
class IPlatform;
class IInput;
class IDebugText;
class ICameraBackend;
class IRendererBackend;

class Application
{
public:
    explicit Application(
        IPlatform& platform, 
        IInput& input, 
        IDebugText& debugText,
        ICameraBackend& cameraBackend, 
        IRendererBackend& rendererBackend);

    ~Application();

    int Run();

private:
    bool Initialize();
    void Shutdown();

    void Update();
    void Render();

private:
    IPlatform& m_platform;
    IInput& m_input;
    IDebugText& m_debugText;
    ICameraBackend& m_cameraBackend;
    IRendererBackend& m_rendererBackend;

    bool m_isInitialized = false;
    bool m_isRunning = false;

private:
    // EN: Application owns the scene coordinator because its lifetime
    //     matches the application's runtime lifecycle.
    //
    // JP: SceneManager の寿命はアプリケーション実行期間と一致するため、
    //     Application が直接所有する。
    SceneManager m_sceneManager;

    ResourceSystem m_resourceSystem;
};