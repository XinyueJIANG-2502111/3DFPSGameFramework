#pragma once

#include "Engine/Math/Vector3.h"

enum class BillboardBlendMode
{
    Alpha,
    Additive
};

struct BillboardRenderData
{
    Vector3 position{};
    float size = 1.0f;
    Vector3 color{ 1.0f };
    float alpha = 1.0f;
};
