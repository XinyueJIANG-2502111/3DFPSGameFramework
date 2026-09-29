#pragma once

#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/ShaderCameraData.h"

#include "Engine/Rendering/Lighting/AmbientLight.h"
#include "Engine/Rendering/Lighting/SpotLight.h"
#include "Engine/Rendering/Lighting/ShaderLightingData.h"

#include "Engine/Rendering/FogSettings.h"
#include "Engine/Rendering/Fog/ShaderFogData.h"

#include "Engine/Rendering/Volumetric/ShaderVolumetricData.h"
#include "Engine/Rendering/Volumetric/VolumetricCone.h"

// forward declaration
class ModelInstance;
class IShaderResource;
struct FogSettings;
struct Vector3;
struct PointLight;
struct SpotLight;
struct AmbientLight;

class DxRenderer final
    : public IRendererBackend
{
public:
    void Draw(
        const ModelInstance& instance) override;

    std::unique_ptr<IModelResource> CreateModelResource(
        const char* filePath) override;

    void SetFog(
        const FogSettings& settings) override;

    void SetClearColor(
        const Vector3& color) override;

    void SetPointLight(
        const PointLight& light) override;

    void SetSpotLight(
        const SpotLight& light) override;

    void SetAmbientLight(
        const AmbientLight& light) override;

    void SetCameraPosition(
        const Vector3& position) override;

    std::unique_ptr<IShaderResource> CreateShaderResource(
        const char* vertexShaderPath,
        const char* pixelShaderPath) override;

    void SetVolumetricSettings(
        const ShaderVolumetricData& settings) override;

    void DrawVolumetricCone(
        const VolumetricCone& cone) override;

public:
    void Shutdown() override;

private:
    int m_spotLightHandle = InvalidHandle;

    AmbientLight m_ambientLight;
    SpotLight m_spotLight;

    ShaderLightingData BuildShaderLightingData() const;

    void EnsureLightingConstantBuffer();
    void UpdateLightingConstantBuffer();

private:
    FogSettings m_fogSettings;

    int m_fogConstantBufferHandle =
        InvalidHandle;

    ShaderFogData BuildShaderFogData() const;

    void EnsureFogConstantBuffer();
    void UpdateFogConstantBuffer();

private:
    static constexpr int InvalidHandle = -1;

    int m_lightingConstantBufferHandle =
        InvalidHandle;

private:
    ShaderCameraData m_cameraData;

    int m_cameraConstantBufferHandle =
        InvalidHandle;

    void EnsureCameraConstantBuffer();
    void UpdateCameraConstantBuffer();

private:
    ShaderVolumetricData m_volumetricData;

    int m_volumetricConstantBufferHandle =
        InvalidHandle;

    void EnsureVolumetricConstantBuffer();
    void UpdateVolumetricConstantBuffer();
};