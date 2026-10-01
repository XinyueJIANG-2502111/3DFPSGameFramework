#pragma once

#include "Engine/Rendering/Camera/ICameraBackend.h"

class DxCamera final : public ICameraBackend
{
public:
    void Apply(const Camera& camera) override;
};