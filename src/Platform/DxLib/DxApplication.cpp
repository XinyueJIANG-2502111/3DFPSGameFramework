#include "Platform/DxLib/DxApplication.h"

#include <DxLib.h>

bool DxApplication::Initialize()
{
    // 使用窗口模式，而不是全屏模式
    ChangeWindowMode(TRUE);

    // 初始化 DxLib
    if (DxLib_Init() == -1)
    {
        return false;
    }

    // 设置后台缓冲区
    SetDrawScreen(DX_SCREEN_BACK);

    return true;
}

void DxApplication::Shutdown()
{
    DxLib_End();
}

bool DxApplication::ProcessEvents()
{
    // 处理 Windows 窗口消息
    if (ProcessMessage() != 0)
    {
        return false;
    }

    return true;
}

void DxApplication::BeginFrame()
{
    // 清空后台缓冲区
    ClearDrawScreen();
}

void DxApplication::EndFrame()
{
    // 将后台缓冲区显示到窗口
    ScreenFlip();
}