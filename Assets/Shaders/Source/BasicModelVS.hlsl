// EN: Minimal DxLib MV1 vertex input for a rigid textured mesh.
// JP: Texture 付き剛体 Mesh 用の最小 DxLib MV1 Vertex Input。
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
    float2 TexCoords0 : TEXCOORD0;
    float4 Diffuse : COLOR0;

    float3 WorldPosition : TEXCOORD1;
    float3 WorldNormal : TEXCOORD2;

    float4 Position : SV_POSITION;
};


// EN: DxLib automatically supplies these matrices when drawing MV1 models.
//
// JP: MV1 Model 描画時に DxLib が自動的に供給する Matrix。
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

cbuffer cbD3D11_CONST_BUFFER_VS_BASE : register(b1)
{
    DX_D3D11_VS_CONST_BUFFER_BASE g_Base;
};


VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    float4 localPosition =
        input.Position;

    // Local -> World
    float4 worldPosition;

    worldPosition.x =
        dot(localPosition, g_Base.LocalWorldMatrix[0]);

    worldPosition.y =
        dot(localPosition, g_Base.LocalWorldMatrix[1]);

    worldPosition.z =
        dot(localPosition, g_Base.LocalWorldMatrix[2]);

    worldPosition.w = 1.0f;

    output.WorldPosition =
        worldPosition.xyz;


    // Local Normal -> World Normal
    float3 worldNormal;

    worldNormal.x =
        dot(
            input.Normal,
            g_Base.LocalWorldMatrix[0].xyz);

    worldNormal.y =
        dot(
            input.Normal,
            g_Base.LocalWorldMatrix[1].xyz);

    worldNormal.z =
        dot(
            input.Normal,
            g_Base.LocalWorldMatrix[2].xyz);

    output.WorldNormal =
        normalize(worldNormal);


    // World -> View
    float4 viewPosition;

    viewPosition.x =
        dot(worldPosition, g_Base.ViewMatrix[0]);

    viewPosition.y =
        dot(worldPosition, g_Base.ViewMatrix[1]);

    viewPosition.z =
        dot(worldPosition, g_Base.ViewMatrix[2]);

    viewPosition.w = 1.0f;


    // View -> Projection
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


    output.TexCoords0 =
        input.TexCoords0.xy;

    output.Diffuse =
        input.Diffuse;

    return output;
}