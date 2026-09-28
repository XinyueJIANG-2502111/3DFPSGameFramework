#include "Platform/DxLib/DxApplication.h"

#include <DxLib.h>

bool DxApplication::Initialize()
{
    ChangeWindowMode(TRUE);

    // MSAA
    SetFullSceneAntiAliasingMode(4, 2);

    if (DxLib_Init() == -1)
    {
        return false;
    }

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
    DxLib_End();
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