struct PS_INPUT
{
    float3 WorldPosition : TEXCOORD0;
};

struct ShaderCameraData
{
    float3 Position;
    float TanHalfFovY;

    float3 Forward;
    float AspectRatio;

    float3 Right;
    float Padding0;

    float3 Up;
    float Padding1;
};

cbuffer CameraBuffer : register(b6)
{
    ShaderCameraData g_Camera;
};

float4 main(
    PS_INPUT input) : SV_TARGET0
{
    const float3 forward =
        normalize(
            g_Camera.Forward);

    const float3 toSurface =
        input.WorldPosition -
        g_Camera.Position;

    // EN: Compute linear depth along the camera forward axis.
    //     This is a view-depth value, not radial camera distance.
    //
    // JP: Camera Forward Axis に沿った Linear Depth を計算する。
    //     Radial Camera Distance ではなく View Depth を表す。
    const float linearViewDepth =
        dot(
            toSurface,
            forward);

    return float4(
        linearViewDepth,
        0.0f,
        0.0f,
        1.0f);
}

// struct PS_INPUT
// {
//     float3 WorldPosition : TEXCOORD0;
// };

// struct ShaderCameraData
// {
//     float3 Position;
//     float Padding0;

//     float3 Forward;
//     float Padding1;
// };

// cbuffer CameraBuffer : register(b6)
// {
//     ShaderCameraData g_Camera;
// };

// // float4 main(
// //     PS_INPUT input) : SV_TARGET0
// // {
// //     // EN: Diagnose whether the camera constant buffer actually
// //     //     changes during the depth pass.
// //     //
// //     // JP: Depth Pass 中に Camera Constant Buffer が実際に
// //     //     更新されているかを診断する。
// //     const float red =
// //         saturate(
// //             g_Camera.Position.x * 0.1f +
// //             0.5f);

// //     const float green =
// //         saturate(
// //             g_Camera.Position.z * 0.1f +
// //             0.5f);

// //     const float blue =
// //         saturate(
// //             g_Camera.Forward.z * 0.5f +
// //             0.5f);

// //     return float4(
// //         red,
// //         green,
// //         blue,
// //         1.0f);
// // }

// // float4 main(
// //     PS_INPUT input) : SV_TARGET0
// // {
// //     // EN: Visualize interpolated world-space position.
// //     //
// //     // JP: 補間された World-Space Position を可視化する。
// //     const float3 debugPosition =
// //         saturate(
// //             input.WorldPosition * 0.1f +
// //             0.5f);

// //     return float4(
// //         debugPosition,
// //         1.0f);
// // }

// float4 main(
//     PS_INPUT input) : SV_TARGET0
// {
//     const float3 forward =
//         normalize(
//             g_Camera.Forward);

//     const float3 toSurface =
//         input.WorldPosition -
//         g_Camera.Position;

//     const float depth =
//         dot(
//             toSurface,
//             forward);


//     // EN: Encode several depth ranges directly as colors.
//     //
//     // JP: Depth Range を直接 Color として Encode する。
//     if (depth <= 0.0f)
//     {
//         return float4(
//             1.0f,
//             0.0f,
//             1.0f,
//             1.0f); // magenta
//     }

//     if (depth < 1.0f)
//     {
//         return float4(
//             1.0f,
//             0.0f,
//             0.0f,
//             1.0f); // red
//     }

//     if (depth < 5.0f)
//     {
//         return float4(
//             0.0f,
//             1.0f,
//             0.0f,
//             1.0f); // green
//     }

//     if (depth < 20.0f)
//     {
//         return float4(
//             0.0f,
//             0.0f,
//             1.0f,
//             1.0f); // blue
//     }

//     return float4(
//         1.0f,
//         1.0f,
//         1.0f,
//         1.0f); // white
// }

// // // EN: Input structure from SceneDepthVS.
// // // JP: SceneDepthVS からの入力構造体。
// // struct PS_INPUT
// // {
// //     float3 WorldPosition : TEXCOORD0;
// // };

// // // EN: Camera constant buffer matching CPU ShaderCameraData (register b6).
// // //     Both Position and Forward are in world space.
// // //
// // // JP: CPU の ShaderCameraData と一致する Camera Constant Buffer (register b6)。
// // //     Position と Forward は共に World Space 座標。
// // struct ShaderCameraData
// // {
// //     float3 Position;
// //     float  Padding0;

// //     float3 Forward;
// //     float  Padding1;
// // };

// // cbuffer CameraBuffer : register(b6)
// // {
// //     ShaderCameraData g_Camera;
// // };

// // float4 main(
// //     PS_INPUT input) : SV_TARGET0
// // {
// //     // EN: Vector from camera position to the surface point in world space.
// //     // JP: World Space におけるカメラ視点位置から表面位置へのベクトル。
// //     const float3 toSurface =
// //         input.WorldPosition -
// //         g_Camera.Position;

// //     // EN: Linear view depth computed by projecting toSurface onto the normalized camera forward vector.
// //     //     This represents the scalar distance along the viewing direction (traditional Z-depth).
// //     //
// //     // JP: toSurface を正規化された Camera Forward ベクトルへ射影して算出した Linear View Depth。
// //     //     視線方向に沿った線形距離（従来の Z 深度）を表す。
// //     const float linearViewDepth =
// //         dot(
// //             toSurface,
// //             normalize(g_Camera.Forward));

// //     // EN: Write the raw linear view depth into the floating-point render target (Red channel).
// //     //     No normalization, remapping, or inversion is performed here.
// //     //
// //     // JP: 生の Linear View Depth を Floating-point Render Target の Red チャンネルに書き込む。
// //     //     ここでは正規化や反転、リマッピング等の加工は行わない。
// //     return float4(
// //         linearViewDepth,
// //         0.0f,
// //         0.0f,
// //         1.0f);
// // }
