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
    // EN: Read the raw linear camera-to-surface distance
    //     stored in the red channel.
    //
    // JP: Red Channel に保存された Raw Linear
    //     Camera-to-Surface Distance を読み取る。
    const float depth =
        g_SceneDepth.Sample(
            g_SceneDepthSampler,
            input.TexCoords0).r;


    // EN: Zero is reserved as the "no geometry" sentinel.
    //
    // JP: 0 は「Geometry なし」を表す Sentinel として使用する。
    if (depth <= 0.0001f)
    {
        return float4(
            0.0f,
            0.0f,
            0.0f,
            1.0f);
    }


    // EN: Debug-only visualization scale.
    //     Larger values make the image darken more quickly with distance.
    //
    // JP: Debug 表示専用の Scale。
    //     値を大きくすると Distance に応じてより早く暗くなる。
    static const float DebugDensity =
        0.1f;


    // EN: Exponential visualization keeps nearby geometry bright
    //     and distant geometry dark without requiring a manually
    //     tuned Near/Far interval.
    //
    // JP: Exponential Visualization により、
    //     Near Geometry を明るく、Far Geometry を暗く表示する。
    //     Near/Far Range の手動調整を必要としない。
    const float visualDepth =
        exp(
            -depth *
            DebugDensity);


    return float4(
        visualDepth,
        visualDepth,
        visualDepth,
        1.0f);
}