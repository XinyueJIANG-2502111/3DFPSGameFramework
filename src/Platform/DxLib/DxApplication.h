#pragma once

#include "Engine/Core/IPlatform.h"

class DxApplication final : public IPlatform
{
public:
    bool Initialize() override;
    void Shutdown() override;

    bool ProcessEvents() override;

    void BeginFrame() override;
    void EndFrame() override;
};