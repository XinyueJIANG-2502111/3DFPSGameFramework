#include <windows.h>

#include "Engine/Core/Application.h"

#include "Platform/DxLib/DxApplication.h"
#include "Platform/DxLib/DxInput.h"
#include "Platform/DxLib/DxDebugText.h"
#include "Platform/DxLib/DxCamera.h"

#include "Platform/DxLib/DxRenderer.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    DxApplication platform;
    DxInput input;
    DxDebugText debugText;
    DxCamera cameraBackend;
    DxRenderer rendererBackend;

    Application app(platform, input, debugText, cameraBackend, rendererBackend);

    return app.Run();
}