#pragma once

#include "Engine/Core/IPlatform.h"

class DxApplication final : public IPlatform
{
public:
    ~DxApplication() override;

    bool Initialize() override;
    void Shutdown() override;

    bool ProcessEvents() override;

    void BeginFrame() override;
    void EndFrame() override;

    ScreenSize GetScreenSize() const override;

private:
    bool m_isInitialized = false;
};