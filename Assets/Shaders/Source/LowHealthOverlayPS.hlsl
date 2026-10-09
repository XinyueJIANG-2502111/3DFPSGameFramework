// EN: Low-health screen overlay.
//     This fullscreen shader draws a soft red vignette near the
//     screen edges and uses vertex alpha from the CPU as the pulse strength.
//
// JP: Low Health 用の Screen Overlay。
//     この Fullscreen Shader は画面端に柔らかい赤い Vignette を描画し、
//     CPU から渡される Vertex Alpha を Pulse 強度として使用する。

struct PS_INPUT
{
    // EN: Keep this layout compatible with the current DxLib 2D shader path.
    //     Do not add COLOR1 here, otherwise TEXCOORD0 may become misaligned.
    //
    // JP: この Layout は現在の DxLib 2D Shader Path と互換性を保つ必要がある。
    //     ここに COLOR1 を追加すると TEXCOORD0 がずれて壊れる可能性がある。
    float4 Position  : SV_POSITION;
    float4 Diffuse   : COLOR0;
    float2 TexCoords0 : TEXCOORD0;
    float2 TexCoords1 : TEXCOORD1;
};

float4 main(PS_INPUT input) : SV_TARGET0
{
    // ------------------------------------------------------------
    // Screen-space coordinate preparation
    // ------------------------------------------------------------

    // EN: Convert UV from [0, 1] into a centered [-1, 1] space.
    //
    // JP: UV を [0, 1] から中心基準の [-1, 1] 空間へ変換する。
    const float2 centeredUv =
        input.TexCoords0 * 2.0f - 1.0f;

    // EN: Use absolute centered coordinates so left/right and
    //     top/bottom behave symmetrically.
    //
    // JP: 左右上下が対称に振る舞うよう、
    //     中心基準座標の絶対値を使用する。
    const float2 centeredAbs =
        abs(centeredUv);

    // ------------------------------------------------------------
    // Border mask
    // ------------------------------------------------------------

    // EN: Box-like edge mask.
    //     0 near the center, 1 near the outer screen border.
    //
    // JP: Box 形状の Edge Mask。
    //     画面中央付近では 0、外周付近で 1 になる。
    float border =
        smoothstep(
            0.55f,
            1.0f,
            max(centeredAbs.x, centeredAbs.y));

    // EN: Add a radial component so the corners feel softer and
    //     the overlay looks less mechanically rectangular.
    //
    // JP: Corner をやわらかく見せ、
    //     Overlay が機械的な四角形に見えすぎないよう
    //     Radial 成分を加える。
    const float radial =
        length(centeredUv);

    border =
        max(
            border,
            smoothstep(
                0.65f,
                1.20f,
                radial));

    // EN: Shape the border slightly so the falloff is softer
    //     near the center and stronger toward the edge.
    //
    // JP: 中央寄りではやわらかく、
    //     外周寄りでは強く見えるように Border を少し整形する。
    border =
        saturate(border * border);

    // ------------------------------------------------------------
    // Pulse intensity from CPU
    // ------------------------------------------------------------

    // EN: The CPU sends pulse strength through vertex alpha.
    //     Clamp it to the valid [0, 1] range.
    //
    // JP: CPU は Vertex Alpha で Pulse 強度を渡す。
    //     有効な [0, 1] 範囲に Clamp する。
    const float pulse =
        saturate(input.Diffuse.a);

    // EN: Final overlay alpha combines the edge mask and pulse strength.
    //
    // JP: 最終的な Overlay Alpha は
    //     Edge Mask と Pulse 強度を組み合わせて求める。
    const float alpha =
        saturate(border * pulse);

    // EN: Dark red tint suitable for low-health feedback.
    //
    // JP: Low Health 演出向けの暗めの赤色。
    const float3 overlayColor =
        float3(0.90f, 0.02f, 0.04f);

    return float4(
        overlayColor,
        alpha);
}