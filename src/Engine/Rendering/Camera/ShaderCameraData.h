#pragma once

#include "Engine/Math/Vector3.h"

#include <cstddef>


//=============================================================================
// ShaderCameraData
//=============================================================================

// EN: GPU-facing camera constant-buffer data.
//
//     The first two 16-byte registers contain the camera data currently used
//     by the scene-depth and camera-ray debug passes.
//
//     Right and Up are already reserved in the layout for the upcoming
//     world-space ray-reconstruction step, but they do not need to be updated
//     until that step is implemented.
//
//     All directional vectors use the Engine world-space convention:
//     +X = Right, +Y = Up, +Z = Forward.
//
// JP: GPU 向けの Camera Constant Buffer Data。
//
//     最初の 2 個の 16 Byte Register には、現在の Scene Depth Pass と
//     Camera Ray Debug Pass が使用する Camera Data を保持する。
//
//     Right と Up は次の World-Space Ray Reconstruction 用として
//     Layout 上に予約しているが、その Step を実装するまでは
//     Runtime 更新を必須としない。
//
//     Direction Vector は Engine の World-Space 座標規約
//     +X = Right, +Y = Up, +Z = Forward に従う。
struct alignas(16) ShaderCameraData
{
    //-------------------------------------------------------------------------
    // Register 0
    //-------------------------------------------------------------------------

    // EN: Camera origin in world space.
    //
    // JP: World Space 上の Camera Origin。
    Vector3 position{
        0.0f,
        0.0f,
        0.0f
    };

    // EN: tan(verticalFov / 2).
    //     Used to reconstruct the vertical extent of camera-space rays.
    //
    // JP: tan(verticalFov / 2)。
    //     Camera-Space Ray の Vertical 範囲を再構築するために使用する。
    float tanHalfFovY =
        1.0f;


    //-------------------------------------------------------------------------
    // Register 1
    //-------------------------------------------------------------------------

    // EN: Normalized camera-forward direction in world space.
    //     Currently used by linear scene-depth calculation.
    //
    // JP: World Space 上の正規化された Camera Forward。
    //     現在は Linear Scene Depth の計算に使用する。
    Vector3 forward{
        0.0f,
        0.0f,
        1.0f
    };

    // EN: Width / height of the active scene render target.
    //     This value belongs to the render target rather than the Camera object.
    //
    // JP: Active Scene Render Target の Width / Height。
    //     Camera Object ではなく Render Target 側に属する値。
    float aspectRatio =
        1.0f;


    //-------------------------------------------------------------------------
    // Register 2
    //-------------------------------------------------------------------------

    // EN: Reserved world-space camera-right vector for the upcoming
    //     world-space ray-reconstruction step.
    //
    // JP: 次の World-Space Ray Reconstruction Step 用として予約する
    //     World-Space Camera Right Vector。
    Vector3 right{
        1.0f,
        0.0f,
        0.0f
    };

    float padding0 =
        0.0f;


    //-------------------------------------------------------------------------
    // Register 3
    //-------------------------------------------------------------------------

    // EN: Reserved world-space camera-up vector for the upcoming
    //     world-space ray-reconstruction step.
    //
    // JP: 次の World-Space Ray Reconstruction Step 用として予約する
    //     World-Space Camera Up Vector。
    Vector3 up{
        0.0f,
        1.0f,
        0.0f
    };

    float padding1 =
        0.0f;
};


//=============================================================================
// GPU layout validation
//=============================================================================

// EN: Engine Vector3 must remain three tightly packed floats because this
//     structure mirrors an HLSL constant-buffer layout.
//
// JP: この Structure は HLSL Constant Buffer Layout と一致させるため、
//     Engine Vector3 は 3 個の Float が連続した 12 Byte である必要がある。
static_assert(
    sizeof(Vector3) == 12,
    "Vector3 must remain 12 bytes for ShaderCameraData.");


// EN: Validate the beginning of every HLSL 16-byte register.
//
// JP: 各 HLSL 16 Byte Register の開始 Offset を検証する。
static_assert(
    offsetof(ShaderCameraData, position) == 0,
    "ShaderCameraData::position layout mismatch.");

static_assert(
    offsetof(ShaderCameraData, tanHalfFovY) == 12,
    "ShaderCameraData::tanHalfFovY layout mismatch.");

static_assert(
    offsetof(ShaderCameraData, forward) == 16,
    "ShaderCameraData::forward layout mismatch.");

static_assert(
    offsetof(ShaderCameraData, aspectRatio) == 28,
    "ShaderCameraData::aspectRatio layout mismatch.");

static_assert(
    offsetof(ShaderCameraData, right) == 32,
    "ShaderCameraData::right layout mismatch.");

static_assert(
    offsetof(ShaderCameraData, up) == 48,
    "ShaderCameraData::up layout mismatch.");


// EN: Four HLSL float4-sized registers = 64 bytes.
//
// JP: HLSL の float4 相当 Register 4 個分 = 64 Byte。
static_assert(
    sizeof(ShaderCameraData) == 64,
    "ShaderCameraData must match the HLSL constant-buffer layout.");

static_assert(
    alignof(ShaderCameraData) == 16,
    "ShaderCameraData must remain 16-byte aligned.");