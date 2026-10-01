#pragma once

#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/Camera/ShaderCameraData.h"

#include "Engine/Rendering/Lighting/AmbientLight.h"
#include "Engine/Rendering/Lighting/SpotLight.h"
#include "Engine/Rendering/Lighting/ShaderLightingData.h"

#include "Engine/Rendering/Fog/FogSettings.h"
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

    void Draw(
        const ModelInstance& instance,
        const Shader& shader) override;

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

    void SetCameraForward(
        const Vector3& forward) override;

    std::unique_ptr<IShaderResource> CreateShaderResource(
        const char* vertexShaderPath,
        const char* pixelShaderPath) override;

    void SetVolumetricSettings(
        const ShaderVolumetricData& settings) override;

    void DrawVolumetricCone(
        const VolumetricCone& cone,
        const Shader& shader) override;

public:
    void BeginSceneRender(
        int width,
        int height) override;

    void EndSceneRender(
        int width,
        int height) override;

    void BeginSceneDepthRender(
        int width,
        int height) override;

    void EndSceneDepthRender() override;

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

private:
    int m_sceneColorHandle =
        InvalidHandle;

    int m_sceneColorWidth = 0;
    int m_sceneColorHeight = 0;

    void EnsureSceneRenderTarget(
        int width,
        int height);

    int m_sceneDepthHandle =
        InvalidHandle;

    int m_sceneDepthWidth = 0;
    int m_sceneDepthHeight = 0;

    bool m_skipSceneDepthDraw = false;
    int m_sceneDepthPreviousBlendMode = 0;
    int m_sceneDepthPreviousBlendParam = 0;

    void EnsureSceneDepthRenderTarget(
        int width,
        int height);

private:
	int m_sceneDepthDebugPixelShaderHandle =
		InvalidHandle;

	void EnsureSceneDepthDebugShader();
	void DrawSceneDepthDebug(
		int width,
		int height);

	Vector3 m_clearColor{
		0.0f,
		0.0f,
		0.0f
	};

private:
    int m_testRenderTargetHandle =
        InvalidHandle;

    void EnsureTestRenderTarget(
        int width,
        int height);

    int m_testRenderTargetWidth = 0;
    int m_testRenderTargetHeight = 0;

};