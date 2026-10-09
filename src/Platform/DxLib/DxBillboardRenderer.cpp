#include "Platform/DxLib/DxRenderer.h"

#include "Engine/Rendering/Texture/Texture.h"
#include "Engine/Rendering/Texture/TextureResourceAccess.h"

#include "Platform/DxLib/DxTextureResource.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <DxLib.h>
#include <vector>

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
    const BillboardDrawSettings& settings)
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
        settings.blendMode == BillboardBlendMode::Additive
            ? DX_BLENDMODE_ADD : DX_BLENDMODE_ALPHA, 255);

    const auto byte = [](float value) {
        return static_cast<unsigned char>(std::clamp(value, 0.0f, 1.0f) * 255.0f);
    };

    int textureHandle = DX_NONE_GRAPH;
    if (settings.texture != nullptr &&
        settings.texture->IsValid())
    {
        const ITextureResource* resource =
            TextureResourceAccess::Get(
                *settings.texture);

        const auto* dxTexture =
            dynamic_cast<const DxTextureResource*>(
                resource);

        if (dxTexture != nullptr &&
            dxTexture->IsValid())
        {
            textureHandle =
                dxTexture->GetGraphHandle();
        }
    }

    // EN: Build the complete batch before submitting it so a burst produces
    //     one DxLib draw call regardless of its particle count.
    // JP: Burst �S�̂� Batch �Ɋ܂߂Ă���A���q���Ɋ֌W�Ȃ�
    //     �P�� DxLib Draw Call �ŕ`�悷��B
    std::vector<VERTEX3D> vertices;
    vertices.reserve(billboards.size() * 6);

    for (const auto& billboard : billboards)
    {
        if (billboard.width <= 0.0f ||
            billboard.height <= 0.0f ||
            billboard.alpha <= 0.0f)
        {
            continue;
        }

        const float cosRotation =
            std::cos(billboard.rotationRadians);
        const float sinRotation =
            std::sin(billboard.rotationRadians);

        const Vector3 rotatedRight =
            m_cameraData.right * cosRotation +
            m_cameraData.up * sinRotation;

        const Vector3 rotatedUp =
            m_cameraData.up * cosRotation -
            m_cameraData.right * sinRotation;

        const Vector3 right =
            rotatedRight * (billboard.width * 0.5f);
        const Vector3 up =
            rotatedUp * (billboard.height * 0.5f);

        const std::array<Vector3, 4> corners{
            billboard.position - right + up,
            billboard.position + right + up,
            billboard.position - right - up,
            billboard.position + right - up
        };

        constexpr std::array<int, 6> indices{ 0, 1, 2, 2, 1, 3 };
        constexpr std::array<float, 4> textureU{ 0.0f, 1.0f, 0.0f, 1.0f };
        constexpr std::array<float, 4> textureV{ 0.0f, 0.0f, 1.0f, 1.0f };

        for (const int index : indices)
        {
            const Vector3& p = corners[index];
            VERTEX3D vertex{};
            vertex.pos = VGet(p.x, p.y, p.z);
            vertex.norm = VGet(-m_cameraData.forward.x,
                -m_cameraData.forward.y, -m_cameraData.forward.z);
            vertex.u = textureU[index];
            vertex.v = textureV[index];
            vertex.dif = GetColorU8(byte(billboard.color.x),
                byte(billboard.color.y), byte(billboard.color.z), byte(billboard.alpha));
            vertices.push_back(vertex);
        }
    }

    if (!vertices.empty())
    {
        DrawPrimitive3D(
            vertices.data(),
            static_cast<int>(vertices.size()),
            DX_PRIMTYPE_TRIANGLELIST,
            textureHandle,
            TRUE);
    }
}
