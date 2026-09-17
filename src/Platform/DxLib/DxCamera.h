#pragma once

#include "Engine/Rendering/ICameraBackend.h"

class DxCamera final : public ICameraBackend
{
public:
    void Apply(const Camera& camera) override;
};