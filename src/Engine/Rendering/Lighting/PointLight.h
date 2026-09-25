#pragma once

#include "Engine/Math/Vector3.h"

struct PointLight
{
    // EN: Controls whether this light contributes to rendering.
    //
    // JP: この Light が Rendering に影響するかどうかを制御する。
    bool enabled = true;

    // EN: World-space position of the light source.
    //
    // JP: Light Source の World Space 上の位置。
    Vector3 position{};

    // EN: Normalized RGB light color.
    //
    // JP: 正規化された RGB Light Color。
    Vector3 color{
        1.0f,
        1.0f,
        1.0f
    };

    // EN: Overall brightness multiplier of the light.
    //
    // JP: Light 全体の明るさを調整する倍率。
    float intensity = 1.0f;

    // EN: Maximum effective distance of the light.
    //
    // JP: Light が影響する最大距離。
    float range = 10.0f;
};