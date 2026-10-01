#pragma once

#include "Engine/Math/Vector3.h"

struct ShaderFogData
{
    // EN: Fog color used by the pixel shader.
    //
    // JP: Pixel Shader が使用する Fog Color。
    Vector3 color{
        0.0f,
        0.0f,
        0.0f
    };

    // EN: Stores whether fog is enabled as a GPU-friendly float.
    //
    // JP: GPU で扱いやすい Float として Fog の有効状態を保持する。
    float enabled = 0.0f;


    // EN: Distance where linear fog begins.
    //
    // JP: Linear Fog が開始する距離。
    float startDistance = 0.0f;

    // EN: Distance where linear fog reaches full strength.
    //
    // JP: Linear Fog が最大強度に達する距離。
    float endDistance = 0.0f;

    // EN: Density used by exponential fog.
    //
    // JP: Exponential Fog で使用する Density。
    float density = 0.0f;

    float padding = 0.0f;
};

static_assert(
    sizeof(ShaderFogData) == 32,
    "ShaderFogData must match the HLSL constant-buffer layout.");