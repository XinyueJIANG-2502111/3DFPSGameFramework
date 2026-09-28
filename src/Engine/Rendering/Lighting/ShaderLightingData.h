#pragma once

#include "Engine/Math/Vector3.h"

struct ShaderAmbientLightData
{
    // EN: Ambient light color already multiplied by intensity.
    //
    // JP: Intensity を適用済みの Ambient Light Color。
    Vector3 color{
        0.0f,
        0.0f,
        0.0f
    };

    // EN: Padding keeps the structure aligned to a 16-byte GPU constant boundary.
    //
    // JP: GPU Constant の 16 Byte 境界に合わせるための Padding。
    float padding = 0.0f;
};


struct ShaderSpotLightData
{
    // EN: World-space position of the spotlight.
    //
    // JP: SpotLight の World Space 上の位置。
    Vector3 position{};

    // EN: Maximum effective distance of the spotlight.
    //
    // JP: SpotLight が影響する最大距離。
    float range = 0.0f;


    // EN: Normalized world-space direction of the spotlight.
    //
    // JP: 正規化された SpotLight の World Space 上の方向。
    Vector3 direction{
        0.0f,
        0.0f,
        1.0f
    };

    // EN: Stores whether this light is enabled as a GPU-friendly float.
    //
    // JP: GPU で扱いやすい Float として Light の有効状態を保持する。
    float enabled = 0.0f;


    // EN: Light color already multiplied by intensity.
    //
    // JP: Intensity を適用済みの Light Color。
    Vector3 color{
        0.0f,
        0.0f,
        0.0f
    };

    float padding0 = 0.0f;


    // EN: Cosine of the inner spotlight cone angle.
    //
    // JP: SpotLight Inner Cone Angle の Cos 値。
    float innerCos = 0.0f;

    // EN: Cosine of the outer spotlight cone angle.
    //
    // JP: SpotLight Outer Cone Angle の Cos 値。
    float outerCos = 0.0f;

    float padding1 = 0.0f;
    float padding2 = 0.0f;
};


struct ShaderLightingData
{
    ShaderAmbientLightData ambient;
    ShaderSpotLightData spotlight;
};