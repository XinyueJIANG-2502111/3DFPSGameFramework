#include "Platform/DxLib/DxApplication.h"

#include <DxLib.h>

DxApplication::~DxApplication()
{
    // EN: Release the initialized DxLib runtime automatically
    //     if explicit application shutdown was not performed.
    //
    // JP: 明示的な Application Shutdown が実行されなかった場合でも、
    //     初期化済みの DxLib Runtime を自動的に解放する。
    Shutdown();
}

bool DxApplication::Initialize()
{
    if (m_isInitialized)
    {
        return true;
    }

    ChangeWindowMode(TRUE);

    // MSAA
    SetFullSceneAntiAliasingMode(4, 2);

    if (DxLib_Init() == -1)
    {
        return false;
    }

    m_isInitialized = true;

    // version check
    const int direct3DVersion =
        GetUseDirect3DVersion();

    const int shaderVersion =
        GetValidShaderVersion();

    const bool isDirect3D11 =
        direct3DVersion == DX_DIRECT3D_11;

    SetDrawScreen(DX_SCREEN_BACK);

    return true;
}

void DxApplication::Shutdown()
{
    // EN: Make shutdown idempotent to support both explicit
    //     shutdown and destructor-based fallback cleanup.
    //
    // JP: 明示的な Shutdown と Destructor による
    //     Fallback Cleanup の両方を安全にするため、
    //     Shutdown を冪等にする。
    if (!m_isInitialized)
    {
        return;
    }

    DxLib_End();

    m_isInitialized = false;
}

bool DxApplication::ProcessEvents()
{
    if (ProcessMessage() != 0)
    {
        return false;
    }

    return true;
}

void DxApplication::BeginFrame()
{
    ClearDrawScreen();
}

void DxApplication::EndFrame()
{
    ScreenFlip();
}

ScreenSize DxApplication::GetScreenSize() const
{
    int width = 0;
    int height = 0;
    int colorBitDepth = 0;

    GetScreenState(
        &width,
        &height,
        &colorBitDepth);

    return ScreenSize{
        width,
        height
    };
}