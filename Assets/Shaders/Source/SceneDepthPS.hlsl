struct PS_INPUT
{
    float3 WorldPosition : TEXCOORD0;
};

struct ShaderCameraData
{
    float3 Position;
    float Padding;
};

cbuffer CameraBuffer : register(b6)
{
    ShaderCameraData g_Camera;
};

float4 main(
    PS_INPUT input) : SV_TARGET0
{
    // EN: Compute the linear world-space distance from the camera
    //     to the current visible surface point.
    //
    // JP: Camera から現在の Visible Surface Point までの
    //     Linear World Space Distance を計算する。
    const float depth =
        length(
            input.WorldPosition -
            g_Camera.Position);

    // EN: Store the raw linear depth in the red channel.
    //     Do not normalize or invert it here because later passes
    //     need the real distance value.
    //
    // JP: Raw Linear Depth を Red Channel に保存する。
    //     後続 Pass では実際の距離値が必要になるため、
    //     ここでは Normalize や反転を行わない。
    return float4(
        depth,
        0.0f,
        0.0f,
        1.0f);
}