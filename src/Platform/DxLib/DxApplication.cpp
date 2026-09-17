#include "Platform/DxLib/DxApplication.h"

#include <DxLib.h>

bool DxApplication::Initialize()
{
    ChangeWindowMode(TRUE);

    if (DxLib_Init() == -1)
    {
        return false;
    }

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