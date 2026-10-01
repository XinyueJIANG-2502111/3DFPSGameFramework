#pragma once

#include "Engine/Math/Vector3.h"

// EN: GPU-facing camera constant buffer data.
//     Both position and forward are expressed in world space.
//     Maintains strict 16-byte alignment matching HLSL register(b6).
//
// JP: GPU 向けの Camera Constant Buffer データ構造体。
//     Position と Forward は共に World Space 座標系で表される。
//     HLSL の register(b6) と一致するよう 16 Byte 単位で厳密にアライメントを保つ。
struct ShaderCameraData
{
    // EN: Camera eye position in world space.
    // JP: World Space 上の Camera 視点位置。
    Vector3 position{
        0.0f,
        0.0f,
        0.0f
    };

    float padding0 = 0.0f;

    // EN: Normalized camera forward direction in world space.
    // JP: World Space 上の正規化された Camera Forward（前方向）ベクトル。
    Vector3 forward{
        0.0f,
        0.0f,
        1.0f
    };

    float padding1 = 0.0f;
};

// EN: Keep the CPU-side structure byte-compatible with the
//     corresponding HLSL constant-buffer layout.
//
// JP: CPU 側の Structure が対応する HLSL Constant Buffer と
//     Byte 単位で一致していることを Compile Time に保証する。
static_assert(
    sizeof(ShaderCameraData) == 32,
    "ShaderCameraData must match the HLSL constant-buffer layout.");