#include "Platform/DxLib/DxRenderer.h"

#include "Engine/Rendering/Model/IModelResource.h"
#include "Engine/Rendering/Model/Model.h"
#include "Engine/Rendering/Model/ModelInstance.h"
#include "Engine/Rendering/Model/ModelResourceAccess.h"

#include "Engine/Rendering/Fog/FogSettings.h"

#include "Engine/Rendering/Lighting/SpotLight.h"
#include "Engine/Rendering/Lighting/AmbientLight.h"

#include "Engine/Math/Matrix4.h"
#include "Engine/Math/Vector3.h"

#include "Engine/Rendering/Shader/Shader.h"
#include "Engine/Rendering/Shader/ShaderResourceAccess.h"

#include "Platform/DxLib/DxModelResource.h"
#include "Platform/DxLib/DxShaderResource.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <DxLib.h>

#ifdef max
#undef max
#endif // max


namespace
{
    MATRIX ToDxMatrix(const Matrix4& matrix)
    {
        MATRIX result{};

        // EN: Engine matrices use column-vector semantics.
        //     Transpose here when crossing the DxLib backend boundary.
        //
        // JP: Engine の Matrix は列ベクトル規約を使用する。
        //     DxLib Backend 境界を越える際にここで転置する。
        for (int row = 0; row < 4; ++row)
        {
            for (int column = 0; column < 4; ++column)
            {
                result.m[row][column] =
                    matrix.m[column][row];
            }
        }

        return result;
    }
}

void DxRenderer::Draw(
    const ModelInstance& instance)
{
    const std::shared_ptr<Model>& model =
        instance.GetModel();

    if (!model || !model->IsValid())
    {
        return;
    }

    const IModelResource* resource =
        ModelResourceAccess::Get(*model);

    const auto* dxResource =
        dynamic_cast<const DxModelResource*>(
            resource);

    if (dxResource == nullptr)
    {
        return;
    }

    const int handle =
        dxResource->GetModel().GetHandle();

    const Matrix4 worldMatrix =
        instance.GetTransform().ToMatrix();

    const MATRIX dxWorldMatrix =
        ToDxMatrix(worldMatrix);

    MV1SetMatrix(
        handle,
        dxWorldMatrix);


    const std::shared_ptr<Shader>& shader =
        instance.GetShader();

    if (shader && shader->IsValid())
    {
        const IShaderResource* shaderResource =
            ShaderResourceAccess::Get(
                *shader);

        const auto* dxShader =
            dynamic_cast<const DxShaderResource*>(
                shaderResource);

        if (dxShader != nullptr)
        {
            // EN: Enable DxLib original-shader mode and bind
            //     the custom vertex/pixel shader pair.
            //
            // JP: DxLib の Original Shader Mode を有効化し、
            //     Custom Vertex / Pixel Shader を Bind する。
            MV1SetUseOrigShader(TRUE);

            SetUseVertexShader(
                dxShader->GetVertexShaderHandle());

            SetUsePixelShader(
                dxShader->GetPixelShaderHandle());

            UpdateLightingConstantBuffer();
            UpdateFogConstantBuffer();
            UpdateCameraConstantBuffer();
            UpdateVolumetricConstantBuffer();
        }
    }

    MV1DrawModel(handle);

    // EN: Restore default DxLib model rendering state after
    //     drawing a custom-shader model.
    //
    // JP: Custom Shader を使用した Model 描画後に、
    //     DxLib の Default Model Rendering State に戻す。
    MV1SetUseOrigShader(FALSE);
    SetUseVertexShader(-1);
    SetUsePixelShader(-1);
}


void DxRenderer::Draw(
    const ModelInstance& instance,
    const Shader& shader)
{
    const std::shared_ptr<Model>& model =
        instance.GetModel();

    if (m_skipSceneDepthDraw ||
        !model ||
        !model->IsValid() ||
        !shader.IsValid())
    {
        return;
    }

    const auto* dxResource =
        dynamic_cast<const DxModelResource*>(
            ModelResourceAccess::Get(
                *model));

    const auto* dxShader =
        dynamic_cast<const DxShaderResource*>(
            ShaderResourceAccess::Get(
                shader));

    if (dxResource == nullptr ||
        dxShader == nullptr ||
        !dxResource->IsValid() ||
        !dxShader->IsValid())
    {
        return;
    }

    const int handle =
        dxResource->GetModel().GetHandle();

    // EN: Enable depth testing and depth writing for this MV1 model
    //     during the scene-depth pass.
    //
    // JP: Scene Depth Pass 中、この MV1 Model に対して
    //     Depth Test と Depth Write を有効にする。
    MV1SetUseZBuffer(
        handle,
        TRUE);

    MV1SetWriteZBuffer(
        handle,
        TRUE);

    const MATRIX dxWorldMatrix =
        ToDxMatrix(
            instance.GetTransform().ToMatrix());

    MV1SetMatrix(
        handle,
        dxWorldMatrix);


    // EN: Upload the latest world-space camera data (position + forward)
    //     used by the pass pixel shader (e.g. scene depth).
    //
    // JP: Pass Pixel Shader（Scene Depth 等）が使用する最新の
    //     World Space Camera Data（Position + Forward）を GPU へ送る。
    UpdateCameraConstantBuffer();


    // EN: Temporarily override the model material shader with
    //     the shader supplied by the current rendering pass.
    //
    // JP: 現在の Rendering Pass から渡された Shader で
    //     Model Material Shader を一時的に置き換える。
    MV1SetUseOrigShader(
        TRUE);

    SetUseVertexShader(
        dxShader->GetVertexShaderHandle());

    SetUsePixelShader(
        dxShader->GetPixelShaderHandle());


    // EN: Execute the actual model draw while the pass-specific
    //     shader pair is active.
    //
    // JP: Pass 専用 Shader が有効な状態で
    //     実際の Model Draw を実行する。
    MV1DrawModel(
        handle);


    // EN: Restore DxLib's normal model-rendering state so the
    //     pass shader cannot leak into later draw calls.
    //
    // JP: Pass Shader が後続 Draw Call に漏れないよう、
    //     DxLib の通常 Model Rendering State に戻す。
    MV1SetUseOrigShader(
        FALSE);

    SetUseVertexShader(-1);
    SetUsePixelShader(-1);
}


std::unique_ptr<IModelResource>
DxRenderer::CreateModelResource(
    const char* filePath)
{
    auto resource =
        std::make_unique<DxModelResource>(
            filePath);

    if (!resource->IsValid())
    {
        return nullptr;
    }

    return resource;
}

void DxRenderer::SetFog(
    const FogSettings& settings)
{
    m_fogSettings = settings;

    if (!settings.enabled)
    {
        SetFogEnable(FALSE);
        return;
    }

    SetFogEnable(TRUE);

    const int red =
        static_cast<int>(
            settings.color.x * 255.0f);

    const int green =
        static_cast<int>(
            settings.color.y * 255.0f);

    const int blue =
        static_cast<int>(
            settings.color.z * 255.0f);

    // EN: Convert the Engine normalized RGB color into the
    //     0-255 integer range expected by DxLib.
    //
    // JP: Engine の正規化 RGB Color を、
    //     DxLib が要求する 0～255 の整数範囲へ変換する。
    SetFogColor(
        red,
        green,
        blue);

    // EN: Linear fog starts at startDistance and reaches
    //     full fog color at endDistance.
    //
    // JP: Linear Fog は startDistance から開始し、
    //     endDistance で Fog Color が最大になる。
    SetFogStartEnd(
        settings.startDistance,
        settings.endDistance);
}

void DxRenderer::SetClearColor(
    const Vector3& color)
{
    m_clearColor.x =
        std::clamp(
            color.x,
            0.0f,
            1.0f);

    m_clearColor.y =
        std::clamp(
            color.y,
            0.0f,
            1.0f);

    m_clearColor.z =
        std::clamp(
            color.z,
            0.0f,
            1.0f);

    SetBackgroundColor(
        static_cast<int>(m_clearColor.x * 255.0f),
        static_cast<int>(m_clearColor.y * 255.0f),
        static_cast<int>(m_clearColor.z * 255.0f));
}

void DxRenderer::SetPointLight(
    const PointLight& light)
{
    // EN: DxLib point-light application will be implemented next.
    //
    // JP: DxLib の Point Light 適用処理は次の Step で実装する。
}

void DxRenderer::SetSpotLight(
    const SpotLight& light)
{
    m_spotLight = light;

    if (!light.enabled)
    {
        if (m_spotLightHandle != -1)
        {
            DeleteLightHandle(
                m_spotLightHandle);

            m_spotLightHandle = -1;
        }

        return;
    }

    if (m_spotLightHandle == -1)
    {
        // EN: Create the DxLib spotlight only when it is first needed.
        //
        // JP: SpotLight が初めて必要になった時だけ
        //     DxLib の Light Handle を生成する。
        m_spotLightHandle =
            CreateSpotLightHandle(
                VGet(
                    light.position.x,
                    light.position.y,
                    light.position.z),
                VGet(
                    light.direction.x,
                    light.direction.y,
                    light.direction.z),
                light.outerAngle,
                light.innerAngle,
                light.range,
                1.0f,
                0.0f,
                0.0f);

        if (m_spotLightHandle == -1)
        {
            return;
        }
    }

    SetLightEnableHandle(
        m_spotLightHandle,
        TRUE);

    SetLightPositionHandle(
        m_spotLightHandle,
        VGet(
            light.position.x,
            light.position.y,
            light.position.z));

    SetLightDirectionHandle(
        m_spotLightHandle,
        VGet(
            light.direction.x,
            light.direction.y,
            light.direction.z));

    // EN: Keep runtime range changes synchronized with the DxLib light.
    //
    // JP: Runtime で変更された Range を
    //     DxLib の Light に同期する。
    SetLightRangeAttenHandle(
        m_spotLightHandle,
        light.range,
        1.0f,
        0.0f,
        0.0f);

    // EN: Keep the spotlight cone angles synchronized with Engine data.
    //
    // JP: SpotLight の Cone Angle を
    //     Engine 側の Data と同期する。
    SetLightAngleHandle(
        m_spotLightHandle,
        light.outerAngle,
        light.innerAngle);

    SetLightDifColorHandle(
        m_spotLightHandle,
        GetColorF(
            light.color.x * light.intensity,
            light.color.y * light.intensity,
            light.color.z * light.intensity,
            1.0f));
}

void DxRenderer::SetAmbientLight(
    const AmbientLight& light)
{
    m_ambientLight = light;

    // EN: Disable DxLib's default directional light so that
    //     custom light handles become the primary scene lighting.
    //
    // JP: Custom Light Handle を Scene の主光源として使用するため、
    //     DxLib の Default Directional Light を無効化する。
    SetLightEnable(FALSE);

    if (!light.enabled)
    {
        SetGlobalAmbientLight(
            GetColorF(
                0.0f,
                0.0f,
                0.0f,
                1.0f));

        return;
    }

    const float red =
        std::clamp(
            light.color.x * light.intensity,
            0.0f,
            1.0f);

    const float green =
        std::clamp(
            light.color.y * light.intensity,
            0.0f,
            1.0f);

    const float blue =
        std::clamp(
            light.color.z * light.intensity,
            0.0f,
            1.0f);

    SetGlobalAmbientLight(
        GetColorF(
            red,
            green,
            blue,
            1.0f));
}

std::unique_ptr<IShaderResource>
DxRenderer::CreateShaderResource(
    const char* vertexShaderPath,
    const char* pixelShaderPath)
{
    auto resource =
        std::make_unique<DxShaderResource>(
            vertexShaderPath,
            pixelShaderPath);

    if (!resource->IsValid())
    {
        return nullptr;
    }

    return resource;
}

ShaderLightingData
DxRenderer::BuildShaderLightingData() const
{
    ShaderLightingData data{};

    // EN: Convert runtime ambient-light state into GPU-facing data.
    //
    // JP: Runtime の Ambient Light State を
    //     GPU 向け Data に変換する。
    if (m_ambientLight.enabled)
    {
        data.ambient.color =
            m_ambientLight.color *
            m_ambientLight.intensity;
    }


    // EN: Convert runtime spotlight state into GPU-facing data.
    //
    // JP: Runtime の SpotLight State を
    //     GPU 向け Data に変換する。
    if (m_spotLight.enabled)
    {
        data.spotlight.enabled = 1.0f;

        data.spotlight.position =
            m_spotLight.position;

        data.spotlight.direction =
            Normalize(m_spotLight.direction);

        data.spotlight.color =
            m_spotLight.color *
            m_spotLight.intensity;

        data.spotlight.range =
            (std::max)(
                m_spotLight.range,
                0.0f);

        data.spotlight.innerCos =
            std::cos(
                m_spotLight.innerAngle);

        data.spotlight.outerCos =
            std::cos(
                m_spotLight.outerAngle);
    }

    return data;
}

void DxRenderer::EnsureLightingConstantBuffer()
{
    if (m_lightingConstantBufferHandle != InvalidHandle)
    {
        return;
    }

    m_lightingConstantBufferHandle =
        CreateShaderConstantBuffer(
            sizeof(ShaderLightingData));
}

void DxRenderer::UpdateLightingConstantBuffer()
{
    EnsureLightingConstantBuffer();

    if (m_lightingConstantBufferHandle == InvalidHandle)
    {
        return;
    }

    void* buffer =
        GetBufferShaderConstantBuffer(
            m_lightingConstantBufferHandle);

    if (buffer == nullptr)
    {
        return;
    }

    const ShaderLightingData data =
        BuildShaderLightingData();

    // EN: Copy CPU-side lighting data into the DxLib
    //     shader constant buffer before uploading it to the GPU.
    //
    // JP: CPU 側の Lighting Data を DxLib の Shader Constant Buffer に
    //     Copy し、GPU へ反映する準備を行う。
    std::memcpy(
        buffer,
        &data,
        sizeof(ShaderLightingData));

    UpdateShaderConstantBuffer(
        m_lightingConstantBufferHandle);

    SetShaderConstantBuffer(
        m_lightingConstantBufferHandle,
        DX_SHADERTYPE_PIXEL,
        4);
}

ShaderFogData
DxRenderer::BuildShaderFogData() const
{
    ShaderFogData data{};

    data.enabled =
        m_fogSettings.enabled ? 1.0f : 0.0f;

    data.color =
        m_fogSettings.color;

    data.startDistance =
        m_fogSettings.startDistance;

    data.endDistance =
        m_fogSettings.endDistance;

    data.density =
        (std::max)(
            m_fogSettings.density,
            0.0f);

    return data;
}

void DxRenderer::EnsureFogConstantBuffer()
{
    if (m_fogConstantBufferHandle != InvalidHandle)
    {
        return;
    }

    m_fogConstantBufferHandle =
        CreateShaderConstantBuffer(
            sizeof(ShaderFogData));
}

void DxRenderer::UpdateFogConstantBuffer()
{
    EnsureFogConstantBuffer();

    if (m_fogConstantBufferHandle == InvalidHandle)
    {
        return;
    }

    void* buffer =
        GetBufferShaderConstantBuffer(
            m_fogConstantBufferHandle);

    if (buffer == nullptr)
    {
        return;
    }

    const ShaderFogData data =
        BuildShaderFogData();

    std::memcpy(
        buffer,
        &data,
        sizeof(ShaderFogData));

    UpdateShaderConstantBuffer(
        m_fogConstantBufferHandle);

    SetShaderConstantBuffer(
        m_fogConstantBufferHandle,
        DX_SHADERTYPE_PIXEL,
        5);
}

void DxRenderer::SetCameraPosition(
    const Vector3& position)
{
    m_cameraData.position =
        position;
}

void DxRenderer::SetCameraForward(
    const Vector3& forward)
{
    // EN: Ensure the forward vector is normalized before storing.
    //     A zero vector safely falls back to default forward (+Z).
    //
    // JP: 保存前に Forward ベクトルが正規化されていることを保証する。
    //     長さ 0 の場合は安全のためデフォルトの前方向 (+Z) とする。
    const float lengthSq = forward.LengthSquared();
    if (lengthSq > 0.0f)
    {
        m_cameraData.forward = Normalize(forward);
    }
    else
    {
        m_cameraData.forward = Vector3{ 0.0f, 0.0f, 1.0f };
    }
}

void DxRenderer::SetCameraRight(
    const Vector3& right)
{
    // EN: Store a normalized world-space camera-right vector.
    //     A zero-length vector falls back to the Engine +X axis.
    //
    // JP: 正規化された World-Space Camera Right Vector を保持する。
    //     長さ 0 の場合は Engine の +X Axis を使用する。
    if (right.LengthSquared() > 0.000001f)
    {
        m_cameraData.right =
            Normalize(right);
    }
    else
    {
        m_cameraData.right =
            Vector3{
                1.0f,
                0.0f,
                0.0f
        };
    }
}

void DxRenderer::SetCameraUp(
    const Vector3& up)
{
    // EN: Store a normalized world-space camera-up vector.
    //     A zero-length vector falls back to the Engine +Y axis.
    //
    // JP: 正規化された World-Space Camera Up Vector を保持する。
    //     長さ 0 の場合は Engine の +Y Axis を使用する。
    if (up.LengthSquared() > 0.000001f)
    {
        m_cameraData.up =
            Normalize(up);
    }
    else
    {
        m_cameraData.up =
            Vector3{
                0.0f,
                1.0f,
                0.0f
        };
    }
}

void DxRenderer::SetCameraFieldOfView(
    float verticalFovRadians)
{
    // EN: Cache the vertical projection scale required by
    //     fullscreen camera-ray reconstruction.
    //
    // JP: Fullscreen Camera-Ray Reconstruction に必要な
    //     Vertical Projection Scale を保持する。
    m_cameraData.tanHalfFovY =
        std::tan(
            verticalFovRadians * 0.5f);
}

void DxRenderer::EnsureCameraConstantBuffer()
{
    if (m_cameraConstantBufferHandle != InvalidHandle)
    {
        return;
    }

    m_cameraConstantBufferHandle =
        CreateShaderConstantBuffer(
            sizeof(ShaderCameraData));
}

void DxRenderer::UpdateCameraConstantBuffer()
{
    EnsureCameraConstantBuffer();

    if (m_cameraConstantBufferHandle == InvalidHandle)
    {
        return;
    }

    void* buffer =
        GetBufferShaderConstantBuffer(
            m_cameraConstantBufferHandle);

    if (buffer == nullptr)
    {
        return;
    }

    // EN: Copy the latest world-space camera data
    //     into the GPU constant buffer.
    //
    // JP: 最新の World Space Camera Data を
    //     GPU Constant Buffer に Copy する。
    std::memcpy(
        buffer,
        &m_cameraData,
        sizeof(ShaderCameraData));

    UpdateShaderConstantBuffer(
        m_cameraConstantBufferHandle);

    SetShaderConstantBuffer(
        m_cameraConstantBufferHandle,
        DX_SHADERTYPE_PIXEL,
        6);
}

void DxRenderer::SetVolumetricSettings(
    const ShaderVolumetricData& settings)
{
    m_volumetricData =
        settings;
}

void DxRenderer::EnsureVolumetricConstantBuffer()
{
    if (m_volumetricConstantBufferHandle != InvalidHandle)
    {
        return;
    }

    m_volumetricConstantBufferHandle =
        CreateShaderConstantBuffer(
            sizeof(ShaderVolumetricData));
}

void DxRenderer::UpdateVolumetricConstantBuffer()
{
    EnsureVolumetricConstantBuffer();

    if (m_volumetricConstantBufferHandle == InvalidHandle)
    {
        return;
    }

    void* buffer =
        GetBufferShaderConstantBuffer(
            m_volumetricConstantBufferHandle);

    if (buffer == nullptr)
    {
        return;
    }

    // EN: Upload the latest volumetric-scattering parameters
    //     to the pixel shader constant buffer.
    //
    // JP: 最新の Volumetric Scattering Parameter を
    //     Pixel Shader Constant Buffer に Upload する。
    std::memcpy(
        buffer,
        &m_volumetricData,
        sizeof(ShaderVolumetricData));

    UpdateShaderConstantBuffer(
        m_volumetricConstantBufferHandle);

    SetShaderConstantBuffer(
        m_volumetricConstantBufferHandle,
        DX_SHADERTYPE_PIXEL,
        7);
}

void DxRenderer::DrawVolumetricCone(
    const VolumetricCone& cone,
    const Shader& shader)
{
    constexpr int SegmentCount = 20;
    constexpr int LayerCount = 3;
    constexpr float TwoPi = 6.28318530718f;

    // ------------------------------------------------------------
    // Validate input
    // ------------------------------------------------------------

    if (cone.range <= 0.0f)
    {
        return;
    }

    const VECTOR rawForward =
        VGet(
            cone.direction.x,
            cone.direction.y,
            cone.direction.z);

    const float forwardLength =
        VSize(rawForward);

    if (forwardLength <= 0.0001f)
    {
        return;
    }


    // ------------------------------------------------------------
    // Access backend shader resource
    // ------------------------------------------------------------

    const IShaderResource* shaderResource =
        ShaderResourceAccess::Get(
            shader);

    const auto* dxShader =
        dynamic_cast<const DxShaderResource*>(
            shaderResource);

    if (dxShader == nullptr ||
        !dxShader->IsValid())
    {
        return;
    }


    // ------------------------------------------------------------
    // Build cone basis
    // ------------------------------------------------------------

    const VECTOR forward =
        VNorm(rawForward);


    // EN: Select a reference axis that is not nearly parallel
    //     to the cone forward direction.
    //
    // JP: Cone の Forward Direction とほぼ平行にならない
    //     Reference Axis を選択する。
    const VECTOR referenceAxis =
        std::abs(forward.y) < 0.99f
        ? VGet(
            0.0f,
            1.0f,
            0.0f)
        : VGet(
            1.0f,
            0.0f,
            0.0f);


    // EN: Construct an orthonormal basis around the cone axis.
    //
    // JP: Cone Axis を中心とした直交 Basis を構築する。
    const VECTOR right =
        VNorm(
            VCross(
                referenceAxis,
                forward));

    const VECTOR up =
        VNorm(
            VCross(
                forward,
                right));


    // ------------------------------------------------------------
    // Build shared cone geometry data
    // ------------------------------------------------------------

    const VECTOR apex =
        VGet(
            cone.position.x,
            cone.position.y,
            cone.position.z);


    // EN: Direction semantics are apex -> beam end,
    //     therefore the cone base lies along +forward.
    //
    // JP: Direction は Apex から Beam End へ向かうため、
    //     Cone Base は +Forward 側に配置する。
    const VECTOR baseCenter =
        VAdd(
            apex,
            VScale(
                forward,
                cone.range));


    VERTEX3DSHADER vertices[
        SegmentCount *
            LayerCount *
            3]{};


        const COLOR_U8 specularColor =
            GetColorU8(
                0,
                0,
                0,
                0);


        struct VolumetricLayer
        {
            float angleScale;
            float weight;
        };


        constexpr VolumetricLayer Layers[
            LayerCount]
        {
            // Outer
            {
                1.00f,
                0.20f
            },

            // Middle
            {
                0.72f,
                0.35f
            },

            // Inner
            {
                0.42f,
                0.55f
            }
        };


        // ------------------------------------------------------------
        // Build nested cone layers
        // ------------------------------------------------------------

        for (int layerIndex = 0;
            layerIndex < LayerCount;
            ++layerIndex)
        {
            const VolumetricLayer& layer =
                Layers[layerIndex];


            // EN: Each nested layer uses a smaller half-angle while
            //     preserving the same origin and beam end distance.
            //
            // JP: 各 Nested Layer は同じ Origin と Beam Distance を保ち、
            //     より小さい Half Angle を使用する。
            const float layerAngle =
                cone.outerAngle *
                layer.angleScale;

            const float radius =
                std::tan(layerAngle) *
                cone.range;


            const float clampedWeight =
                (std::clamp)(
                    layer.weight,
                    0.0f,
                    1.0f);

            const unsigned char layerAlpha =
                static_cast<unsigned char>(
                    clampedWeight *
                    255.0f);


            const COLOR_U8 diffuseColor =
                GetColorU8(
                    255,
                    255,
                    255,
                    layerAlpha);


            for (int i = 0;
                i < SegmentCount;
                ++i)
            {
                const float angle0 =
                    TwoPi *
                    static_cast<float>(i) /
                    static_cast<float>(
                        SegmentCount);

                const float angle1 =
                    TwoPi *
                    static_cast<float>(i + 1) /
                    static_cast<float>(
                        SegmentCount);


                const VECTOR radial0 =
                    VAdd(
                        VScale(
                            right,
                            std::cos(angle0) *
                            radius),
                        VScale(
                            up,
                            std::sin(angle0) *
                            radius));


                const VECTOR radial1 =
                    VAdd(
                        VScale(
                            right,
                            std::cos(angle1) *
                            radius),
                        VScale(
                            up,
                            std::sin(angle1) *
                            radius));


                const VECTOR base0 =
                    VAdd(
                        baseCenter,
                        radial0);

                const VECTOR base1 =
                    VAdd(
                        baseCenter,
                        radial1);


                const int triangleIndex =
                    layerIndex *
                    SegmentCount +
                    i;

                const int vertexIndex =
                    triangleIndex *
                    3;


                VERTEX3DSHADER& v0 =
                    vertices[
                        vertexIndex + 0];

                VERTEX3DSHADER& v1 =
                    vertices[
                        vertexIndex + 1];

                VERTEX3DSHADER& v2 =
                    vertices[
                        vertexIndex + 2];


                // ----------------------------------------------------
                // Position
                // ----------------------------------------------------

                v0.pos = apex;
                v1.pos = base0;
                v2.pos = base1;


                // ----------------------------------------------------
                // Normal
                // ----------------------------------------------------

                // EN: The current volumetric shader does not use
                //     geometric normals yet, but VERTEX3DSHADER still
                //     receives a valid fallback normal.
                //
                // JP: 現在の Volumetric Shader では Geometry Normal を
                //     使用しないが、有効な Fallback Normal を設定する。
                const VECTOR dummyNormal =
                    VScale(
                        forward,
                        -1.0f);

                v0.norm = dummyNormal;
                v1.norm = dummyNormal;
                v2.norm = dummyNormal;


                // ----------------------------------------------------
                // Vertex color
                // ----------------------------------------------------

                v0.dif = diffuseColor;
                v1.dif = diffuseColor;
                v2.dif = diffuseColor;

                v0.spc = specularColor;
                v1.spc = specularColor;
                v2.spc = specularColor;


                // ----------------------------------------------------
                // Texture coordinates
                // ----------------------------------------------------

                v0.u = 0.0f;
                v0.v = 0.0f;
                v0.su = 0.0f;
                v0.sv = 0.0f;

                v1.u = 0.0f;
                v1.v = 0.0f;
                v1.su = 0.0f;
                v1.sv = 0.0f;

                v2.u = 0.0f;
                v2.v = 0.0f;
                v2.su = 0.0f;
                v2.sv = 0.0f;
            }
        }


        // ------------------------------------------------------------
        // Render state
        // ------------------------------------------------------------

        int previousBlendMode = 0;
        int previousBlendParam = 0;

        GetDrawBlendMode(
            &previousBlendMode,
            &previousBlendParam);

        const int previousBackCulling =
            GetUseBackCulling();


        // EN: The camera may be inside the volume, so both sides of
        //     the cone shell must remain visible during this V1 pass.
        //
        // JP: Camera が Volume 内部に入る可能性があるため、
        //     V1 Pass では Cone Shell を両面描画する。
        SetUseBackCulling(
            FALSE);


        // EN: Test against scene depth, but do not let this transparent
        //     volume overwrite the depth buffer.
        //
        // JP: Scene Depth との判定は行うが、
        //     半透明 Volume 自体は Depth Buffer に書き込まない。
        SetDepthTest(
            TRUE);

        SetDepthWrite(
            FALSE);


        SetDrawBlendMode(
            DX_BLENDMODE_ALPHA,
            160);


        // ------------------------------------------------------------
        // Activate volumetric shader
        // ------------------------------------------------------------

        SetUseVertexShader(
            dxShader->GetVertexShaderHandle());

        SetUsePixelShader(
            dxShader->GetPixelShaderHandle());


        // EN: The volumetric shader uses both the current spotlight
        //     state and dedicated volumetric parameters.
        //
        // JP: Volumetric Shader は現在の SpotLight State と
        //     専用 Volumetric Parameter の両方を使用する。
        UpdateLightingConstantBuffer();
        UpdateVolumetricConstantBuffer();


        // ------------------------------------------------------------
        // Draw
        // ------------------------------------------------------------

        DrawPolygon3DToShader(
            vertices,
            SegmentCount *
            LayerCount);


        // ------------------------------------------------------------
        // Clear shader state
        // ------------------------------------------------------------

        SetUseVertexShader(-1);
        SetUsePixelShader(-1);


        // ------------------------------------------------------------
        // Restore render state
        // ------------------------------------------------------------

        SetDrawBlendMode(
            previousBlendMode,
            previousBlendParam);

        SetUseBackCulling(
            previousBackCulling);


        // EN: Current renderer code assumes normal scene rendering
        //     uses depth testing and depth writing after this pass.
        //
        // JP: 現在の Renderer では、この Pass 後の通常描画が
        //     Depth Test と Depth Write を使用する前提で戻す。
        SetDepthTest(
            TRUE);

        SetDepthWrite(
            TRUE);
}

void DxRenderer::EnsureSceneRenderTarget(
    int width,
    int height)
{
    if (width <= 0 ||
        height <= 0)
    {
        return;
    }

    // EN: Reuse the current render target when its dimensions
    //     already match the requested scene size.
    //
    // JP: 現在の Render Target Size が要求された Scene Size と
    //     一致する場合は既存 Resource を再利用する。
    if (m_sceneColorHandle != InvalidHandle &&
        m_sceneColorWidth == width &&
        m_sceneColorHeight == height)
    {
        return;
    }


    // EN: Destroy the old render target before recreating it
    //     with the new dimensions.
    //
    // JP: 新しい Size で再作成する前に、
    //     古い Render Target を解放する。
    if (m_sceneColorHandle != InvalidHandle)
    {
        DeleteGraph(
            m_sceneColorHandle);

        m_sceneColorHandle =
            InvalidHandle;

        m_sceneColorWidth = 0;
        m_sceneColorHeight = 0;
    }


    m_sceneColorHandle =
        MakeScreen(
            width,
            height,
            TRUE);

    if (m_sceneColorHandle == InvalidHandle)
    {
        return;
    }

    m_sceneColorWidth =
        width;

    m_sceneColorHeight =
        height;
}

void DxRenderer::BeginSceneRender(
    int width,
    int height)
{
    m_isSceneDepthPass = false;
    // EN: Aspect ratio belongs to the active render target.
    //     Cache it here so fullscreen shaders use the dimensions
    //     of the surface currently used for scene rendering.
    //
    // JP: Aspect Ratio は Active Render Target に属する値なので、
    //     Scene Rendering に使用する Surface Size からここで計算する。
    if (height > 0)
    {
        m_cameraData.aspectRatio =
            static_cast<float>(width) /
            static_cast<float>(height);
    }
    else
    {
        m_cameraData.aspectRatio =
            1.0f;
    }


    EnsureSceneRenderTarget(
        width,
        height);

    EnsureSceneDepthRenderTarget(
        width,
        height);


    if (m_sceneColorHandle ==
        InvalidHandle)
    {
        return;
    }


    SetDrawScreen(
        m_sceneColorHandle);

    ClearDrawScreen();
    SetDepthTest(true);
    SetDepthWrite(true);
    SetLighting(true);
}

void DxRenderer::EndSceneRender(
    int width,
    int height)
{
    if (m_sceneColorHandle == InvalidHandle)
    {
        return;
    }

    SetDrawScreen(
        DX_SCREEN_BACK);

    DrawExtendGraph(
        0,
        0,
        width,
        height,
        m_sceneColorHandle,
        FALSE);
}

void DxRenderer::EnsureSceneDepthRenderTarget(
    int width,
    int height)
{
    if (width <= 0 ||
        height <= 0)
    {
        return;
    }

    if (m_sceneDepthHandle != InvalidHandle &&
        m_sceneDepthWidth == width &&
        m_sceneDepthHeight == height)
    {
        return;
    }

    if (m_sceneDepthHandle != InvalidHandle)
    {
        DeleteGraph(
            m_sceneDepthHandle);

        m_sceneDepthHandle =
            InvalidHandle;

        m_sceneDepthWidth = 0;
        m_sceneDepthHeight = 0;
    }


    // EN: Create a floating-point render target for linear scene depth.
    //     Floating-point storage avoids the severe precision loss of
    //     an ordinary 8-bit color render target.
    //
    // JP: Linear Scene Depth を保存するために
    //     Floating-point Render Target を作成する。
    //     通常の 8-bit Color Render Target による
    //     深度精度の低下を避ける。
    const int previousFloatType = GetDrawValidFloatTypeGraphCreateFlag();
    const int previousChannelNum = GetCreateDrawValidGraphChannelNum();
    const int previousBitDepth = GetCreateGraphChannelBitDepth();
    const int previousZBuffer = GetDrawValidGraphCreateZBufferFlag();

    SetDrawValidGraphCreateZBufferFlag(TRUE);

    
    SetDrawValidFloatTypeGraphCreateFlag(TRUE);

    SetCreateDrawValidGraphChannelNum(4);

    SetCreateGraphChannelBitDepth(32);


    m_sceneDepthHandle =
        MakeScreen(
            width,
            height,
            TRUE);


    // EN: Creation settings are global; restore the caller's values even
    //     when allocation fails. The depth target owns its native Z-buffer.
    //
    // JP: 作成設定はグローバルなので、確保失敗時も呼び出し前の値へ戻す。
    //     深度ターゲットには最前面を選ぶための専用 Z バッファを持たせる。
    SetDrawValidFloatTypeGraphCreateFlag(previousFloatType);
    SetCreateDrawValidGraphChannelNum(previousChannelNum);
    SetCreateGraphChannelBitDepth(previousBitDepth);
    SetDrawValidGraphCreateZBufferFlag(previousZBuffer);


    if (m_sceneDepthHandle == InvalidHandle)
    {
        return;
    }

    m_sceneDepthWidth =
        width;

    m_sceneDepthHeight =
        height;
}

void DxRenderer::BeginSceneDepthRender(
    int width,
    int height)
{
    m_isSceneDepthPass = true;
    m_skipSceneDepthDraw = true;

    EnsureSceneDepthRenderTarget(
        width,
        height);

    if (m_sceneDepthHandle ==
        InvalidHandle)
    {
        return;
    }

    if (SetDrawScreen(
        m_sceneDepthHandle) == -1)
    {
        return;
    }

    m_skipSceneDepthDraw = false;

    //// EN: Diagnostic step.
    ////     Use the offscreen scene-depth render target while keeping
    ////     all other rendering state identical to the known-working
    ////     back-buffer test.
    ////
    //// JP: 診断用 Step。
    ////     動作確認済みの Back Buffer Test と同じ State を維持したまま、
    ////     Offscreen Scene Depth Render Target のみを使用する。
    //if (SetDrawScreen(
    //    m_sceneDepthHandle) == -1)
    //{
    //    return;
    //}

    SetDrawBlendMode(
        DX_BLENDMODE_NOBLEND,
        0);

    SetBackgroundColor(
        0,
        0,
        0);

    ClearDrawScreen();

	// EN: SetBackgroundColor is global DxLib state.
    //     Restore the normal scene clear color immediately after
    //     clearing the depth-as-color target.
    //
    // JP: SetBackgroundColor は DxLib の Global State なので、
    //     Depth-as-Color Target の Clear 後に通常 Scene 用の
    //     Clear Color を直ちに復元する。
	SetBackgroundColor(
		static_cast<int>(
			m_clearColor.x * 255.0f),
		static_cast<int>(
			m_clearColor.y * 255.0f),
		static_cast<int>(
			m_clearColor.z * 255.0f));
}

void DxRenderer::EndSceneDepthRender()
{
    // EN: Finish only the linear scene-depth pass here.
    //     Fullscreen volumetric composition is executed by its own
    //     explicit render pass.
    //
    // JP: ここでは Linear Scene Depth Pass の終了処理だけを行う。
    //     Fullscreen Volumetric Composition は独立した
    //     Render Pass として別途実行する。
    m_isSceneDepthPass = false;

    SetDrawScreen(
        DX_SCREEN_BACK);

    m_skipSceneDepthDraw = false;
}

void DxRenderer::RenderVolumetricLighting(
    int width,
    int height)
{
    // EN: Volumetric composition requires both the opaque scene color
    //     and the linear scene-depth textures.
    //
    // JP: Volumetric Composition には Opaque Scene Color と
    //     Linear Scene Depth の両方が必要となる。
    if (width <= 0 ||
        height <= 0 ||
        m_sceneColorHandle == InvalidHandle ||
        m_sceneDepthHandle == InvalidHandle)
    {
        return;
    }

    // EN: The fullscreen shader performs scene-color plus volumetric
    //     composition itself, therefore GPU additive blending must not
    //     add the scene color a second time.
    //
    // JP: Fullscreen Shader 内で Scene Color と Volumetric Lighting を
    //     合成するため、GPU 側で Additive Blend を重ねてはならない。
    SetDrawScreen(
        DX_SCREEN_BACK);

    SetDrawBlendMode(
        DX_BLENDMODE_NOBLEND,
        0);

    DrawCameraRayDebug(
        width,
        height);
}

void DxRenderer::EnsureSceneDepthDebugShader()
{
    if (m_sceneDepthDebugPixelShaderHandle !=
        InvalidHandle)
    {
        return;
    }

    // EN: Load the temporary pixel shader used to visualize
    //     the linear scene-depth render target.
    //
    // JP: Linear Scene Depth Render Target を可視化するための
    //     Temporary Pixel Shader を読み込む。
    m_sceneDepthDebugPixelShaderHandle =
        LoadPixelShader(
            "Assets/Shaders/Source/SceneDepthDebugPS.pso");
}

void DxRenderer::DrawSceneDepthDebug(
    int width,
    int height)
{
    if (m_sceneDepthHandle == InvalidHandle ||
        width <= 0 ||
        height <= 0)
    {
        return;
    }

    EnsureSceneDepthDebugShader();

    if (m_sceneDepthDebugPixelShaderHandle ==
        InvalidHandle)
    {
        return;
    }


    VERTEX2DSHADER vertices[6]{};

    const COLOR_U8 white =
        GetColorU8(
            255,
            255,
            255,
            255);

    const COLOR_U8 black =
        GetColorU8(
            0,
            0,
            0,
            0);


    const float left =
        -0.5f;

    const float top =
        -0.5f;

    const float right =
        static_cast<float>(width) -
        0.5f;

    const float bottom =
        static_cast<float>(height) -
        0.5f;


    auto setVertex =
        [&](VERTEX2DSHADER& vertex,
            float x,
            float y,
            float u,
            float v)
        {
            vertex.pos =
                VGet(
                    x,
                    y,
                    0.0f);

            vertex.rhw =
                1.0f;

            vertex.dif =
                white;

            vertex.spc =
                black;

            vertex.u = u;
            vertex.v = v;

            vertex.su = u;
            vertex.sv = v;
        };


    // Triangle 1
    setVertex(
        vertices[0],
        left,
        top,
        0.0f,
        0.0f);

    setVertex(
        vertices[1],
        right,
        top,
        1.0f,
        0.0f);

    setVertex(
        vertices[2],
        left,
        bottom,
        0.0f,
        1.0f);


    // Triangle 2
    setVertex(
        vertices[3],
        left,
        bottom,
        0.0f,
        1.0f);

    setVertex(
        vertices[4],
        right,
        top,
        1.0f,
        0.0f);

    setVertex(
        vertices[5],
        right,
        bottom,
        1.0f,
        1.0f);


    // EN: Bind the scene depth-as-color render target as texture slot 0.
    //
    // JP: Scene Depth-as-Color Render Target を
    //     Texture Slot 0 に Bind する。
    SetUseTextureToShader(
        0,
        m_sceneDepthHandle);

    SetUsePixelShader(
        m_sceneDepthDebugPixelShaderHandle);


    // EN: Render two screen-space triangles covering the
    //     entire output surface.
    //
    // JP: Output Surface 全体を覆う
    //     Screen Space Triangle を 2 枚描画する。
    DrawPolygon2DToShader(
        vertices,
        2);


    // EN: Clear temporary shader bindings so they do not leak
    //     into later render operations.
    //
    // JP: 後続 Rendering に影響しないよう
    //     Temporary Shader Binding を解除する。
    SetUsePixelShader(-1);

    SetUseTextureToShader(
        0,
        -1);
}

void DxRenderer::EnsureCameraRayDebugShader()
{
    if (m_cameraRayDebugPixelShaderHandle !=
        InvalidHandle)
    {
        return;
    }

    // EN: Load the temporary fullscreen shader used to
    //     validate screen-space ray coordinates.
    //
    // JP: Screen-Space Ray Coordinate を検証するための
    //     Temporary Fullscreen Shader を読み込む。
    m_cameraRayDebugPixelShaderHandle =
        LoadPixelShader(
            "Assets/Shaders/Source/CameraRayDebugPS.pso");
}

void DxRenderer::DrawCameraRayDebug(
    int width,
    int height)
{
    if (width <= 0 ||
        height <= 0)
    {
        return;
    }

    EnsureCameraRayDebugShader();

    if (m_cameraRayDebugPixelShaderHandle ==
        InvalidHandle)
    {
        return;
    }


    VERTEX2DSHADER vertices[6]{};

    const COLOR_U8 white =
        GetColorU8(
            255,
            255,
            255,
            255);

    const COLOR_U8 black =
        GetColorU8(
            0,
            0,
            0,
            0);


    const float left =
        -0.5f;

    const float top =
        -0.5f;

    const float right =
        static_cast<float>(width) -
        0.5f;

    const float bottom =
        static_cast<float>(height) -
        0.5f;


    auto setVertex =
        [&](VERTEX2DSHADER& vertex,
            float x,
            float y,
            float u,
            float v)
        {
            vertex.pos =
                VGet(
                    x,
                    y,
                    0.0f);

            vertex.rhw =
                1.0f;

            vertex.dif =
                white;

            vertex.spc =
                black;

            vertex.u = u;
            vertex.v = v;

            vertex.su = u;
            vertex.sv = v;
        };


    setVertex(
        vertices[0],
        left,
        top,
        0.0f,
        0.0f);

    setVertex(
        vertices[1],
        right,
        top,
        1.0f,
        0.0f);

    setVertex(
        vertices[2],
        left,
        bottom,
        0.0f,
        1.0f);


    setVertex(
        vertices[3],
        left,
        bottom,
        0.0f,
        1.0f);

    setVertex(
        vertices[4],
        right,
        top,
        1.0f,
        0.0f);

    setVertex(
        vertices[5],
        right,
        bottom,
        1.0f,
        1.0f);


    // EN: Upload the latest camera and projection parameters before
    //     the fullscreen shader reconstructs camera-space rays.
    //
    // JP: Fullscreen Shader が Camera-Space Ray を再構築する前に、
    //     最新の Camera / Projection Parameter を GPU へ反映する。
    UpdateCameraConstantBuffer();


    // EN: Upload the current lighting state so the fullscreen
    //     debug pass can access the spotlight cone parameters.
    //
    // JP: Fullscreen Debug Pass から Spotlight Cone Parameter を
    //     参照できるよう、現在の Lighting State を GPU へ反映する。
    UpdateLightingConstantBuffer();

    // EN: Upload volumetric parameters used by the fullscreen
    //     analytical scattering debug pass.
    //
    // JP: Fullscreen Analytical Scattering Debug Pass で使用する
    //     Volumetric Parameter を GPU へ反映する。
    UpdateVolumetricConstantBuffer();


    // EN: Bind the linear scene-depth render target so the fullscreen
    //     shader can convert view depth into ray distance.
    //
    // JP: View Depth を Ray Distance に変換できるよう、
    //     Linear Scene Depth Render Target を Fullscreen Shader に Bind する。
    SetUseTextureToShader(
        0,
        m_sceneDepthHandle);

    // EN: Bind the normal scene-color render target for final
    //     fullscreen volumetric composition.
    //
    // JP: 最終的な Fullscreen Volumetric Composition のために、
    //     通常の Scene Color Render Target を Bind する。
    SetUseTextureToShader(
        1,
        m_sceneColorHandle);

    SetUsePixelShader(
        m_cameraRayDebugPixelShaderHandle);

    DrawPolygon2DToShader(
        vertices,
        2);

    SetUsePixelShader(-1);
    SetUseTextureToShader(
        0,
        -1);
    SetUseTextureToShader(
        1,
        -1);
}

void DxRenderer::Shutdown()
{
    if (m_spotLightHandle != InvalidHandle)
    {
        DeleteLightHandle(
            m_spotLightHandle);

        m_spotLightHandle =
            InvalidHandle;
    }

    if (m_lightingConstantBufferHandle != InvalidHandle)
    {
        DeleteShaderConstantBuffer(
            m_lightingConstantBufferHandle);

        m_lightingConstantBufferHandle =
            InvalidHandle;
    }

    if (m_fogConstantBufferHandle != InvalidHandle)
    {
        DeleteShaderConstantBuffer(
            m_fogConstantBufferHandle);

        m_fogConstantBufferHandle =
            InvalidHandle;
    }

    if (m_cameraConstantBufferHandle != InvalidHandle)
    {
        DeleteShaderConstantBuffer(
            m_cameraConstantBufferHandle);

        m_cameraConstantBufferHandle =
            InvalidHandle;
    }

    if (m_volumetricConstantBufferHandle != InvalidHandle)
    {
        DeleteShaderConstantBuffer(
            m_volumetricConstantBufferHandle);

        m_volumetricConstantBufferHandle =
            InvalidHandle;
    }

    if (m_sceneColorHandle != InvalidHandle)
    {
        DeleteGraph(
            m_sceneColorHandle);

        m_sceneColorHandle =
            InvalidHandle;

        m_sceneColorWidth = 0;
        m_sceneColorHeight = 0;
    }

    if (m_sceneDepthHandle != InvalidHandle)
    {
        DeleteGraph(
            m_sceneDepthHandle);

        m_sceneDepthHandle =
            InvalidHandle;

        m_sceneDepthWidth = 0;
        m_sceneDepthHeight = 0;
    }

    if (m_sceneDepthDebugPixelShaderHandle !=
        InvalidHandle)
    {
        DeleteShader(
            m_sceneDepthDebugPixelShaderHandle);

        m_sceneDepthDebugPixelShaderHandle =
            InvalidHandle;
    }

    if (m_cameraRayDebugPixelShaderHandle !=
        InvalidHandle)
    {
        DeleteShader(
            m_cameraRayDebugPixelShaderHandle);

        m_cameraRayDebugPixelShaderHandle =
            InvalidHandle;
    }
}
