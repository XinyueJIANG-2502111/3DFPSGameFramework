#pragma once

struct ShaderVolumetricData
{
    // EN: Enables or disables the volumetric scattering effect.
    //
    // JP: Volumetric Scattering Effect の有効状態を保持する。
    float enabled = 0.0f;

    // EN: Controls the overall brightness of the visible light beam.
    //
    // JP: 可視光線の全体的な Brightness を制御する。
    float intensity = 1.0f;

    // EN: Controls how strongly the medium scatters flashlight light.
    //
    // JP: Medium が Flashlight の Light をどれだけ散乱させるかを制御する。
    float scattering = 0.05f;

    // EN: Reserved for later ray-marching or temporal tuning.
    //
    // JP: 後で Ray Marching や Temporal 調整に使用するための予約領域。
    float padding = 0.0f;
};

static_assert(
    sizeof(ShaderVolumetricData) == 16,
    "ShaderVolumetricData must match the HLSL constant-buffer layout.");