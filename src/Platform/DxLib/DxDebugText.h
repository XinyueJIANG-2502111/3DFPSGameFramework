#pragma once

#include "Engine/Debug/IDebugText.h"

class DxDebugText final : public IDebugText
{
public:
    void Draw(
        int x,
        int y,
        const char* text) override;
};