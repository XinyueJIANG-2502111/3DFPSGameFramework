#pragma once

#include "Engine/Math/Vector3.h"

struct AmbientLight
{
    // EN: Controls whether global ambient lighting is applied.
    //
    // JP: Global Ambient Lighting を適用するかどうかを制御する。
    bool enabled = true;

    // EN: Normalized RGB ambient light color.
    //
    // JP: 正規化された RGB Ambient Light Color。
    Vector3 color{
        0.05f,
        0.05f,
        0.05f
    };

    // EN: Overall brightness multiplier of the ambient light.
    //
    // JP: Ambient Light 全体の明るさを調整する倍率。
    float intensity = 1.0f;
};