#pragma once

#include "Engine/Math/Vector3.h"

struct FogSettings
{
    // EN: Controls whether distance fog is enabled.
    //
    // JP: 距離 Fog を有効にするかどうかを制御する。
    bool enabled = false;

    // EN: Fog color stored as normalized RGB values in the [0, 1] range.
    //
    // JP: [0, 1] の範囲に正規化された RGB 値として Fog Color を保持する。
    Vector3 color{
        0.5f,
        0.5f,
        0.5f
    };

    // EN: Distance from the camera where linear fog starts to affect objects.
    //
    // JP: Linear Fog が Object に影響し始める Camera からの距離。
    float startDistance = 10.0f;

    // EN: Distance from the camera where linear fog reaches full strength.
    //
    // JP: Linear Fog が最大強度に達する Camera からの距離。
    float endDistance = 50.0f;
};