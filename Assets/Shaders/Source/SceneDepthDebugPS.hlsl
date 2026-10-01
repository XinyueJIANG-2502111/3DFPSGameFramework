struct PS_INPUT
{
    float4 Diffuse    : COLOR0;
    float4 Specular   : COLOR1;
    float2 TexCoords0 : TEXCOORD0;
    float2 TexCoords1 : TEXCOORD1;
};

Texture2D<float4> g_SceneDepth
    : register(t0);

SamplerState g_SceneDepthSampler
    : register(s0);

float4 main(
    PS_INPUT input) : SV_TARGET0
{
    const float depth =
        g_SceneDepth.Sample(
            g_SceneDepthSampler,
            input.TexCoords0).r;

    if (depth <= 0.0001f)
    {
        return float4(
            0.0f,
            0.0f,
            0.0f,
            1.0f);
    }

    // EN: Debug-only depth range.
    //     Tune this to the current test scene.
    //
    // JP: Debug 表示専用の Depth Range。
    //     現在の Test Scene に合わせて調整する。
    static const float DebugNear =
        0.0f;

    static const float DebugFar =
        10.0f;

    const float normalizedDepth =
        saturate(
            (depth - DebugNear) /
            (DebugFar - DebugNear));

    const float visualDepth =
        1.0f -
        normalizedDepth;

    return float4(
        visualDepth,
        visualDepth,
        visualDepth,
        1.0f);
}


// // EN: Input vertex attributes for fullscreen quad or diagnostic rendering.
// // JP: 全画面 Quad または診断描画用の入力頂点属性。
// struct PS_INPUT
// {
//     float4 Diffuse    : COLOR0;
//     float4 Specular   : COLOR1;
//     float2 TexCoords0 : TEXCOORD0;
//     float2 TexCoords1 : TEXCOORD1;
// };

// // EN: Linear scene-depth texture bound at register t0.
// // JP: register t0 にバインドされる Linear Scene Depth テクスチャ。
// Texture2D<float4> g_SceneDepth
//     : register(t0);

// SamplerState g_SceneDepthSampler
//     : register(s0);

// // EN: Diagnostic visualization range for depth in world units.
// // JP: ワールド単位での深度診断用可視化範囲。
// static const float DebugNear = 0.5f;
// static const float DebugFar  = 20.0f;

// float4 main(
//     PS_INPUT input) : SV_TARGET0
// {
//     // EN: Sample the raw linear view depth from the red channel.
//     // JP: Red チャンネルから生の Linear View Depth をサンプリングする。
//     const float depth =
//         g_SceneDepth.Sample(
//             g_SceneDepthSampler,
//             input.TexCoords0).r;

//     // EN: Values <= 0.0001 represent invalid pixels (cleared background or behind the camera).
//     //     Treat them as black.
//     //
//     // JP: 0.0001 以下の値は無効ピクセル（クリアされた背景またはカメラ後方）を表す。
//     //     黒として描画する。
//     if (depth <= 0.0001f)
//     {
//         return float4(
//             0.0f,
//             0.0f,
//             0.0f,
//             1.0f);
//     }

//     // EN: Linear normalization mapped to [0, 1] range.
//     // JP: [0, 1] の範囲に線形正規化する。
//     const float normalizedDepth =
//         saturate(
//             (depth - DebugNear) /
//             (DebugFar - DebugNear));

//     // EN: Invert for intuitive grayscale visualization (near = white, far = black).
//     // JP: 直感的なグレースケール可視化のため反転（手前が白、奥が黒）。
//     const float visualDepth =
//         1.0f -
//         normalizedDepth;

//     return float4(
//         visualDepth,
//         visualDepth,
//         visualDepth,
//         1.0f);
// }
