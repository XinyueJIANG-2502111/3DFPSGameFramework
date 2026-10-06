#include "Platform/DxLib/DxRenderer.h"

#include <algorithm>
#include <array>
#include <DxLib.h>

void DxRenderer::SetDepthTest(bool enabled)
{
    SetUseZBuffer3D(enabled ? TRUE : FALSE);
    m_depthTest = enabled;
}

void DxRenderer::SetDepthWrite(bool enabled)
{
    SetWriteZBuffer3D(enabled ? TRUE : FALSE);
    m_depthWrite = enabled;
}

void DxRenderer::SetLighting(bool enabled)
{
    SetUseLighting(enabled ? TRUE : FALSE);
    m_lighting = enabled;
}

void DxRenderer::DrawBillboards(
    std::span<const BillboardRenderData> billboards,
    BillboardBlendMode blendMode)
{
    if (billboards.empty() || m_isSceneDepthPass || m_skipSceneDepthDraw)
    {
        return;
    }

    // EN: DxLib has no depth/lighting getters. The backend tracks all its
    //     changes and establishes these states at the start of the color pass.
    // JP: DxLib に深度・照明状態の取得 API がないため、Backend が変更を追跡し、
    //     Color Pass の開始時に状態を確立する。
    struct StateRestore
    {
        DxRenderer& renderer;
        bool depthTest;
        bool depthWrite;
        bool lighting;
        int blendMode = 0;
        int blendParam = 0;
        int culling = GetUseBackCulling();
        int fog = GetFogEnable();

        explicit StateRestore(DxRenderer& owner)
            : renderer(owner), depthTest(owner.m_depthTest),
              depthWrite(owner.m_depthWrite), lighting(owner.m_lighting)
        {
            GetDrawBlendMode(&blendMode, &blendParam);
        }

        ~StateRestore()
        {
            renderer.SetDepthTest(depthTest);
            renderer.SetDepthWrite(depthWrite);
            renderer.SetLighting(lighting);
            SetDrawBlendMode(blendMode, blendParam);
            SetUseBackCulling(culling);
            SetFogEnable(fog);
        }
    } restore(*this);

    // EN: Opaque walls must occlude sparks, but transparent quads must not
    //     occlude each other or alter the independent linear SceneDepth pass.
    // JP: 壁は Spark を遮蔽するが、透明 Quad 同士の遮蔽や独立した
    //     Linear SceneDepth Pass への書き込みは行わない。
    SetDepthTest(true);
    SetDepthWrite(false);
    SetLighting(false);
    SetFogEnable(FALSE);
    SetUseBackCulling(DX_CULLING_NONE);
    SetDrawBlendMode(
        blendMode == BillboardBlendMode::Additive
            ? DX_BLENDMODE_ADD : DX_BLENDMODE_ALPHA, 255);

    const auto byte = [](float value) {
        return static_cast<unsigned char>(std::clamp(value, 0.0f, 1.0f) * 255.0f);
    };
    for (const auto& billboard : billboards)
    {
        if (billboard.size <= 0.0f || billboard.alpha <= 0.0f)
        {
            continue;
        }
        const Vector3 right = m_cameraData.right * (billboard.size * 0.5f);
        const Vector3 up = m_cameraData.up * (billboard.size * 0.5f);
        const std::array<Vector3, 4> corners{
            billboard.position - right + up,
            billboard.position + right + up,
            billboard.position - right - up,
            billboard.position + right - up
        };
        constexpr std::array<int, 6> indices{ 0, 1, 2, 2, 1, 3 };
        std::array<VERTEX3D, 6> vertices{};
        for (size_t i = 0; i < vertices.size(); ++i)
        {
            const Vector3& p = corners[indices[i]];
            vertices[i].pos = VGet(p.x, p.y, p.z);
            vertices[i].norm = VGet(-m_cameraData.forward.x,
                -m_cameraData.forward.y, -m_cameraData.forward.z);
            vertices[i].dif = GetColorU8(byte(billboard.color.x),
                byte(billboard.color.y), byte(billboard.color.z), byte(billboard.alpha));
        }
        // EN: Standard untextured primitives leave custom shader/texture
        //     bindings untouched; only ToShader draws consume those bindings.
        // JP: 通常の無テクスチャ Primitive は Custom Shader / Texture の
        //     Binding を変更しない。Binding は ToShader 描画のみで使われる。
        DrawPrimitive3D(vertices.data(), static_cast<int>(vertices.size()),
            DX_PRIMTYPE_TRIANGLELIST, DX_NONE_GRAPH, TRUE);
    }
}
