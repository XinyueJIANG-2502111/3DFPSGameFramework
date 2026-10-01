#pragma once

#include "Engine/Math/Vector3.h"

struct ShaderCameraData
{
    // EN: Camera position in world space.
    //
    // JP: World Space ã‚Ì Camera PositionB
    Vector3 position{
        0.0f,
        0.0f,
        0.0f
    };

    float padding = 0.0f;
};

// EN: Keep the CPU-side structure byte-compatible with the
//     corresponding HLSL constant-buffer layout.
//
// JP: CPU ‘¤‚Ì Structure ‚ª‘Î‰‚·‚é HLSL Constant Buffer ‚Æ
//     Byte ’PˆÊ‚Åˆê’v‚µ‚Ä‚¢‚é‚±‚Æ‚ğ Compile Time ‚É•ÛØ‚·‚éB
static_assert(
    sizeof(ShaderCameraData) == 16,
    "ShaderCameraData must match the HLSL constant-buffer layout.");