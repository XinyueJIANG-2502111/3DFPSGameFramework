struct PS_INPUT
{
    float4 Diffuse       : COLOR0;
    float3 WorldPosition : TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 Color : SV_TARGET0;
};


// ------------------------------------------------------------
// Spotlight
// ------------------------------------------------------------

struct ShaderSpotLightData
{
    float3 Position;
    float Range;

    float3 Direction;
    float Enabled;

    float3 Color;
    float Padding0;

    float InnerCos;
    float OuterCos;
    float Padding1;
    float Padding2;
};

struct ShaderAmbientLightData
{
    float3 Color;
    float Padding;
};

cbuffer LightingBuffer : register(b4)
{
    ShaderAmbientLightData g_Ambient;
    ShaderSpotLightData g_SpotLight;
};


// ------------------------------------------------------------
// Volumetric
// ------------------------------------------------------------

struct ShaderVolumetricData
{
    float Enabled;
    float Intensity;
    float Scattering;
    float Padding;
};

cbuffer VolumetricBuffer : register(b7)
{
    ShaderVolumetricData g_Volumetric;
};


PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;

    if (g_Volumetric.Enabled <= 0.5f ||
        g_SpotLight.Enabled <= 0.5f)
    {
        output.Color =
            float4(
                0.0f,
                0.0f,
                0.0f,
                0.0f);

        return output;
    }


    // EN: Measure how far the current cone surface point is
    //     from the flashlight origin.
    //
    // JP: 現在の Cone Surface Point が Flashlight Origin から
    //     どれだけ離れているかを計算する。
    const float distanceFromLight =
        length(
            input.WorldPosition -
            g_SpotLight.Position);


    const float safeRange =
        max(
            g_SpotLight.Range,
            0.0001f);


    // EN: Normalize distance into the 0..1 flashlight range.
    //
    // JP: Flashlight Range 内の距離を 0..1 に正規化する。
    const float normalizedDistance =
        saturate(
            distanceFromLight /
            safeRange);


    // EN: Fade the visible beam as it travels away from the source.
    //     Squaring produces a smoother falloff than a purely
    //     linear attenuation.
    //
    // JP: Light Source から離れるほど Visible Beam を減衰させる。
    //     二乗することで単純な Linear Attenuation より
    //     滑らかな減衰にする。
    float distanceFade =
        1.0f -
        normalizedDistance;

    distanceFade *=
        distanceFade;


    const float intensity =
        max(
            g_Volumetric.Intensity,
            0.0f);

    const float scattering =
        max(
            g_Volumetric.Scattering,
            0.0f);


    // EN: Use the actual flashlight color rather than a hard-coded
    //     debug beam color.
    //
    // JP: 固定 Debug Color ではなく、実際の Flashlight Color を
    //     Volumetric Beam に使用する。
    const float3 beamColor =
        g_SpotLight.Color;


    // EN: Vertex alpha stores the contribution weight of the
    //     current nested volumetric layer.
    //
    // JP: Vertex Alpha には現在の Nested Volumetric Layer の
    //     Contribution Weight を保持する。
    const float layerWeight =
        saturate(
            input.Diffuse.a);


    const float beamAlpha =
        saturate(
            scattering *
            intensity *
            distanceFade *
            layerWeight);


    output.Color.rgb =
        beamColor *
        intensity *
        distanceFade;

    output.Color.a =
        beamAlpha;

    return output;
}