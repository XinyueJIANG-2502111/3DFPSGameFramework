struct VS_INPUT
{
    float4 Position : POSITION;
    float3 Normal : NORMAL0;
    float4 Diffuse : COLOR0;
    float4 Specular : COLOR1;
    float4 TexCoords0 : TEXCOORD0;
    float4 TexCoords1 : TEXCOORD1;
    int4 BlendIndices0 : BLENDINDICES0;
    float4 BlendWeight0 : BLENDWEIGHT0;
};

struct VS_OUTPUT
{
    float3 WorldPosition : TEXCOORD0;
    float4 Position : SV_POSITION;
};

struct DX_D3D11_VS_CONST_BUFFER_BASE
{
    float4 AntiViewportMatrix[4];
    float4 ProjectionMatrix[4];
    float4 ViewMatrix[3];
    float4 LocalWorldMatrix[3];

    float4 ToonOutLineSize;

    float DiffuseSource;
    float SpecularSource;
    float MulSpecularColor;
    float Padding;
};

cbuffer cbD3D11_CONST_BUFFER_VS_BASE
    : register(b1)
{
    DX_D3D11_VS_CONST_BUFFER_BASE g_Base;
};

VS_OUTPUT main(
    VS_INPUT input)
{
    VS_OUTPUT output;


    // EN: Build a homogeneous local-space position explicitly.
    //     Do not rely on the incoming POSITION.w value.
    //
    // JP: Homogeneous Local Position を明示的に構築する。
    //     入力 POSITION.w の値には依存しない。
    float4 localPosition;

    localPosition.xyz =
        input.Position.xyz;

    localPosition.w =
        1.0f;


    // EN: Transform the local-space vertex position into world space.
    //
    // JP: Local Space の頂点座標を World Space に変換する。
    float4 worldPosition;

    worldPosition.x =
        dot(
            localPosition,
            g_Base.LocalWorldMatrix[0]);

    worldPosition.y =
        dot(
            localPosition,
            g_Base.LocalWorldMatrix[1]);

    worldPosition.z =
        dot(
            localPosition,
            g_Base.LocalWorldMatrix[2]);

    worldPosition.w =
        1.0f;


    // EN: Pass the true world-space surface position to the pixel shader.
    //     The rasterizer interpolates this value across the triangle.
    //
    // JP: 正しい World Space Surface Position を Pixel Shader に渡す。
    //     Rasterizer が Triangle 内でこの値を補間する。
    output.WorldPosition =
        worldPosition.xyz;


    // EN: Transform world space into view space.
    //
    // JP: World Space から View Space へ変換する。
    float4 viewPosition;

    viewPosition.x =
        dot(
            worldPosition,
            g_Base.ViewMatrix[0]);

    viewPosition.y =
        dot(
            worldPosition,
            g_Base.ViewMatrix[1]);

    viewPosition.z =
        dot(
            worldPosition,
            g_Base.ViewMatrix[2]);

    viewPosition.w =
        1.0f;


    // EN: Transform view space into clip space.
    //
    // JP: View Space から Clip Space へ変換する。
    output.Position.x =
        dot(
            viewPosition,
            g_Base.ProjectionMatrix[0]);

    output.Position.y =
        dot(
            viewPosition,
            g_Base.ProjectionMatrix[1]);

    output.Position.z =
        dot(
            viewPosition,
            g_Base.ProjectionMatrix[2]);

    output.Position.w =
        dot(
            viewPosition,
            g_Base.ProjectionMatrix[3]);


    return output;
}