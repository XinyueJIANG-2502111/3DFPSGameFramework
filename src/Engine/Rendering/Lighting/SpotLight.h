#pragma once

#include "Engine/Math/Vector3.h"

struct SpotLight
{
    // EN: Controls whether this light contributes to rendering.
    //
    // JP: この Light が Rendering に影響するかどうかを制御する。
    bool enabled = true;

    // EN: World-space origin of the spotlight.
    //
    // JP: SpotLight の World Space 上の発光位置。
    Vector3 position{};

    // EN: World-space direction in which the spotlight points.
    //
    // JP: SpotLight が向いている World Space 上の方向。
    Vector3 direction{
        0.0f,
        0.0f,
        1.0f
    };

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

    // EN: Maximum effective distance of the spotlight.
    //
    // JP: SpotLight が影響する最大距離。
    float range = 15.0f;

    // EN: Fully illuminated inner cone angle in radians.
    //
    // JP: 完全に照らされる Inner Cone の角度。単位は Radian。
    float innerAngle = 0.30f;

    // EN: Outer cone angle where the spotlight reaches zero intensity.
    //
    // JP: SpotLight の明るさが 0 になる Outer Cone の角度。単位は Radian。
    float outerAngle = 0.50f;
};