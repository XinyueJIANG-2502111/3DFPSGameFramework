#include <windows.h>

#include "Engine/Core/Application.h"

#include "Platform/DxLib/DxApplication.h"
#include "Platform/DxLib/DxInput.h"
#include "Platform/DxLib/DxDebugText.h"
#include "Platform/DxLib/DxCamera.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    DxApplication platform;
    DxInput input;
    DxDebugText debugText;
    DxCamera cameraBackend;

    Application app(platform, input, debugText, cameraBackend);

    return app.Run();
}