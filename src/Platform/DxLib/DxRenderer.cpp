#include "Platform/DxLib/DxRenderer.h"

#include "Engine/Rendering/IModelResource.h"
#include "Engine/Rendering/Model.h"
#include "Engine/Rendering/ModelInstance.h"
#include "Engine/Rendering/ModelResourceAccess.h"

#include "Engine/Rendering/FogSettings.h"

#include "Engine/Rendering/Lighting/SpotLight.h"
#include "Engine/Rendering/Lighting/AmbientLight.h"

#include "Engine/Math/Matrix4.h"
#include "Engine/Math/Vector3.h"

#include "Engine/Rendering/Shader.h"
#include "Engine/Rendering/ShaderResourceAccess.h"

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
    const float red =
        std::clamp(color.x, 0.0f, 1.0f);

    const float green =
        std::clamp(color.y, 0.0f, 1.0f);

    const float blue =
        std::clamp(color.z, 0.0f, 1.0f);

    SetBackgroundColor(
        static_cast<int>(red * 255.0f),
        static_cast<int>(green * 255.0f),
        static_cast<int>(blue * 255.0f));
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

        m_fogConstantBufferHandle =
            InvalidHandle;

        m_cameraConstantBufferHandle =
            InvalidHandle;
    }
}

void DxRenderer::SetCameraPosition(
    const Vector3& position)
{
    m_cameraData.position =
        position;
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