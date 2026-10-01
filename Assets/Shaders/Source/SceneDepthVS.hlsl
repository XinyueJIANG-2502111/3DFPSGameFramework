// EN: Input vertex structure from DxLib MV1 model.
// JP: DxLib MV1 Model からの入力頂点構造体。
struct VS_INPUT
{
    float4 Position      : POSITION;
    float3 Normal        : NORMAL0;
    float4 Diffuse       : COLOR0;
    float4 Specular      : COLOR1;
    float4 TexCoords0    : TEXCOORD0;
    float4 TexCoords1    : TEXCOORD1;
    int4   BlendIndices0 : BLENDINDICES0;
    float4 BlendWeight0  : BLENDWEIGHT0;
};

// EN: Output vertex structure for scene depth pass.
//     Passes world-space position to the pixel shader so depth can be computed
//     relative to the engine-supplied camera position and forward vector.
//
// JP: Scene Depth Pass 用の頂点シェーダー出力構造体。
//     Pixel Shader で Engine から渡された Camera Position と Forward ベクトルに基づき
//     深度を計算できるよう、World Space 座標を渡す。
struct VS_OUTPUT
{
    float3 WorldPosition : TEXCOORD0;
    float4 Position      : SV_POSITION;
};

// EN: Standard DxLib matrix constant buffer in register b1.
// JP: DxLib の標準マトリクス定数バッファ (register b1)。
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

    // EN: Construct homogeneous local-space position.
    // JP: 斉次ローカル空間座標を構築する。
    float4 localPosition =
        float4(
            input.Position.xyz,
            1.0f);

    // ------------------------------------------------------------
    // Local -> World
    // ------------------------------------------------------------
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

    // EN: Output true world-space position.
    //     Linear view depth is calculated in the pixel shader using this position.
    //
    // JP: 実際の World Space 座標を出力する。
    //     Pixel Shader 側でこの座標を用いて Linear View Depth を算出する。
    output.WorldPosition =
        worldPosition.xyz;

    // ------------------------------------------------------------
    // World -> View
    // ------------------------------------------------------------
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

    // ------------------------------------------------------------
    // View -> Clip (Rasterization position)
    // ------------------------------------------------------------
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
