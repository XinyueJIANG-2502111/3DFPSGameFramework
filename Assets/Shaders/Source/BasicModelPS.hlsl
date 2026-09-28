struct PS_INPUT
{
    float2 TexCoords0 : TEXCOORD0;
    float4 Diffuse : COLOR0;

    float3 WorldPosition : TEXCOORD1;
    float3 WorldNormal : TEXCOORD2;
};

struct PS_OUTPUT
{
    float4 Color : SV_TARGET0;
};


// EN: Ambient light data packed to one 16-byte constant-buffer slot.
//
// JP: 1 �� 16 Byte Constant Buffer Slot �Ɏ��܂�
//     Ambient Light Data�B
struct ShaderAmbientLightData
{
    float3 Color;
    float Padding;
};


// EN: Spotlight data packed to four 16-byte constant-buffer slots.
//
// JP: 4 �� 16 Byte Constant Buffer Slot �ɕ����ĕێ�����
//     SpotLight Data�B
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


// EN: Custom lighting constants supplied by the Engine.
//
// JP: Engine �����狟������� Custom Lighting Constant�B
cbuffer LightingBuffer : register(b4)
{
    ShaderAmbientLightData g_Ambient;
    ShaderSpotLightData g_SpotLight;
};

struct ShaderFogData
{
    float3 Color;
    float Enabled;

    float StartDistance;
    float EndDistance;
    float Density;
    float Padding;
};

cbuffer FogBuffer : register(b5)
{
    ShaderFogData g_Fog;
};

struct ShaderCameraData
{
    float3 Position;
    float Padding;
};

// EN: Camera state supplied by the renderer.
//
// JP: Renderer から供給される Camera State。
cbuffer CameraBuffer : register(b6)
{
    ShaderCameraData g_Camera;
};


Texture2D g_DiffuseMapTexture
    : register(t0);

SamplerState g_DiffuseMapSampler
    : register(s0);


PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;

    const float4 textureColor =
        g_DiffuseMapTexture.Sample(
            g_DiffuseMapSampler,
            input.TexCoords0);

    const float4 baseColor =
        textureColor * input.Diffuse;

    const float3 normal =
        normalize(input.WorldNormal);


    // ------------------------------------------------------------
    // Lighting
    // ------------------------------------------------------------

    float3 lighting =
        g_Ambient.Color;

    if (g_SpotLight.Enabled > 0.5f)
    {
        const float3 surfaceToLight =
            g_SpotLight.Position -
            input.WorldPosition;

        const float distanceToLight =
            length(surfaceToLight);

        if (distanceToLight < g_SpotLight.Range)
        {
            const float3 lightDirection =
                surfaceToLight /
                max(distanceToLight, 0.0001f);

            const float diffuseFactor =
                saturate(
                    dot(
                        normal,
                        lightDirection));

            float distanceAttenuation =
                saturate(
                    1.0f -
                    distanceToLight /
                    max(
                        g_SpotLight.Range,
                        0.0001f));

            distanceAttenuation *=
                distanceAttenuation;

            const float3 lightToSurface =
                -lightDirection;

            const float coneCos =
                dot(
                    normalize(g_SpotLight.Direction),
                    lightToSurface);

            const float coneRange =
                max(
                    g_SpotLight.InnerCos -
                    g_SpotLight.OuterCos,
                    0.0001f);

            const float coneFactor =
                saturate(
                    (coneCos -
                        g_SpotLight.OuterCos) /
                    coneRange);

            const float spotFactor =
                diffuseFactor *
                distanceAttenuation *
                coneFactor;

            lighting +=
                g_SpotLight.Color *
                spotFactor;
        }
    }


    // float3 surfaceColor =
    //     baseColor.rgb * lighting;

    float3 surfaceColor =
    saturate(
        baseColor.rgb * lighting);


    // ------------------------------------------------------------
    // Linear Fog
    // ------------------------------------------------------------

    // if (g_Fog.Enabled > 0.5f)
    // {
    //     // EN: Approximate camera distance using world-space position.
    //     //
    //     // JP: World Space Position を使用して
    //     //     Camera からの距離を近似する。
    //     const float fogDistance =
    //         length(input.WorldPosition - g_Camera.Position);

    //     const float fogRange =
    //         max(
    //             g_Fog.EndDistance -
    //             g_Fog.StartDistance,
    //             0.0001f);

    //     const float fogFactor =
    //         saturate(
    //             (fogDistance -
    //                 g_Fog.StartDistance) /
    //             fogRange);

    //     surfaceColor =
    //         lerp(
    //             surfaceColor,
    //             g_Fog.Color,
    //             fogFactor);
    // }

    // ------------------------------------------------------------
    // Exponential Fog
    // ------------------------------------------------------------

    if (g_Fog.Enabled > 0.5f)
    {
        // EN: Compute the world-space distance from the camera
        //     to the current surface.
        //
        // JP: Camera から現在の Surface までの
        //     World Space Distance を計算する。
        const float cameraDistance =
            length(
                input.WorldPosition -
                g_Camera.Position);


        // EN: Fog starts accumulating only after StartDistance.
        //
        // JP: StartDistance を超えた位置から
        //     Fog の蓄積を開始する。
        const float fogDistance =
            max(
                cameraDistance -
                    g_Fog.StartDistance,
                0.0f);


        // EN: Exponential transmittance.
        //     At distance 0 the scene is fully visible.
        //     Visibility decreases exponentially as distance increases.
        //
        // JP: Exponential Fog の透過率。
        //     Distance が 0 の場合は完全に見え、
        //     距離が増えるほど指数関数的に Visibility が低下する。
        const float transmittance =
            exp(
                -g_Fog.Density *
                fogDistance);


        // EN: Convert remaining visibility into fog intensity.
        //
        // JP: 残っている Visibility から Fog 強度を求める。
        const float fogFactor =
            saturate(
                1.0f -
                transmittance);


        surfaceColor =
            lerp(
                surfaceColor,
                g_Fog.Color,
                fogFactor);
    }


        output.Color.rgb =
            surfaceColor;

    output.Color.a =
        baseColor.a;

    return output;
}