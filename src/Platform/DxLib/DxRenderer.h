#pragma once

#include "Engine/Rendering/Renderer.h"

#include "Engine/Math/Vector3.h"

#include "Engine/Rendering/Camera/ShaderCameraData.h"

#include "Engine/Rendering/Lighting/AmbientLight.h"
#include "Engine/Rendering/Lighting/SpotLight.h"
#include "Engine/Rendering/Lighting/ShaderLightingData.h"

#include "Engine/Rendering/Fog/FogSettings.h"
#include "Engine/Rendering/Fog/ShaderFogData.h"

#include "Engine/Rendering/Volumetric/ShaderVolumetricData.h"
#include "Engine/Rendering/Volumetric/VolumetricCone.h"


//=============================================================================
// Forward declarations
//=============================================================================

class ModelInstance;
class IShaderResource;
class Shader;

struct PointLight;


//=============================================================================
// DxRenderer
//=============================================================================

class DxRenderer final
    : public IRendererBackend
{
public:
    //-------------------------------------------------------------------------
    // Resource creation
    //-------------------------------------------------------------------------

    std::unique_ptr<IModelResource> CreateModelResource(
        const char* filePath) override;

    std::unique_ptr<IShaderResource> CreateShaderResource(
        const char* vertexShaderPath,
        const char* pixelShaderPath) override;


    //-------------------------------------------------------------------------
    // Render passes
    //-------------------------------------------------------------------------

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


    //-------------------------------------------------------------------------
    // Camera shader data
    //-------------------------------------------------------------------------

    void SetCameraPosition(
        const Vector3& position) override;

    void SetCameraForward(
        const Vector3& forward) override;

    void SetCameraRight(
        const Vector3& right) override;

    void SetCameraUp(
        const Vector3& up) override;

    void SetCameraFieldOfView(
        float verticalFovRadians) override;


    //-------------------------------------------------------------------------
    // Scene rendering state
    //-------------------------------------------------------------------------

    void SetClearColor(
        const Vector3& color) override;

    void SetFog(
        const FogSettings& settings) override;

    void SetAmbientLight(
        const AmbientLight& light) override;

    void SetPointLight(
        const PointLight& light) override;

    void SetSpotLight(
        const SpotLight& light) override;


    //-------------------------------------------------------------------------
    // Model drawing
    //-------------------------------------------------------------------------

    void Draw(
        const ModelInstance& instance) override;

    void Draw(
        const ModelInstance& instance,
        const Shader& shader) override;


    //-------------------------------------------------------------------------
    // Volumetric rendering
    //-------------------------------------------------------------------------

    void SetVolumetricSettings(
        const ShaderVolumetricData& settings) override;

    void DrawVolumetricCone(
        const VolumetricCone& cone,
        const Shader& shader) override;


    //-------------------------------------------------------------------------
    // Lifecycle
    //-------------------------------------------------------------------------

    // EN: Releases all rendering resources owned by the DxLib backend
    //     before the graphics platform itself is shut down.
    //
    // JP: Graphics Platform 自体が終了する前に、
    //     DxLib Backend が所有する Rendering Resource を解放する。
    void Shutdown() override;


private:
    static constexpr int InvalidHandle =
        -1;


    //-------------------------------------------------------------------------
    // Lighting
    //-------------------------------------------------------------------------

    int m_spotLightHandle =
        InvalidHandle;

    AmbientLight m_ambientLight;
    SpotLight m_spotLight;

    int m_lightingConstantBufferHandle =
        InvalidHandle;

    ShaderLightingData BuildShaderLightingData() const;

    void EnsureLightingConstantBuffer();
    void UpdateLightingConstantBuffer();


    //-------------------------------------------------------------------------
    // Fog
    //-------------------------------------------------------------------------

    FogSettings m_fogSettings;

    int m_fogConstantBufferHandle =
        InvalidHandle;

    ShaderFogData BuildShaderFogData() const;

    void EnsureFogConstantBuffer();
    void UpdateFogConstantBuffer();


    //-------------------------------------------------------------------------
    // Camera shader data
    //-------------------------------------------------------------------------

    ShaderCameraData m_cameraData;

    int m_cameraConstantBufferHandle =
        InvalidHandle;

    void EnsureCameraConstantBuffer();
    void UpdateCameraConstantBuffer();


    //-------------------------------------------------------------------------
    // Volumetric rendering
    //-------------------------------------------------------------------------

    ShaderVolumetricData m_volumetricData;

    int m_volumetricConstantBufferHandle =
        InvalidHandle;

    void EnsureVolumetricConstantBuffer();
    void UpdateVolumetricConstantBuffer();


    //-------------------------------------------------------------------------
    // Scene color render target
    //-------------------------------------------------------------------------

    int m_sceneColorHandle =
        InvalidHandle;

    int m_sceneColorWidth =
        0;

    int m_sceneColorHeight =
        0;

    void EnsureSceneRenderTarget(
        int width,
        int height);


    //-------------------------------------------------------------------------
    // Scene depth render target
    //-------------------------------------------------------------------------

    int m_sceneDepthHandle =
        InvalidHandle;

    int m_sceneDepthWidth =
        0;

    int m_sceneDepthHeight =
        0;

    bool m_skipSceneDepthDraw =
        false;

    int m_sceneDepthPreviousBlendMode =
        0;

    int m_sceneDepthPreviousBlendParam =
        0;

    void EnsureSceneDepthRenderTarget(
        int width,
        int height);


    //-------------------------------------------------------------------------
    // Scene depth debug
    //-------------------------------------------------------------------------

    int m_sceneDepthDebugPixelShaderHandle =
        InvalidHandle;

    void EnsureSceneDepthDebugShader();

    void DrawSceneDepthDebug(
        int width,
        int height);


    //-------------------------------------------------------------------------
    // Camera ray debug
    //-------------------------------------------------------------------------

    int m_cameraRayDebugPixelShaderHandle =
        InvalidHandle;

    void EnsureCameraRayDebugShader();

    void DrawCameraRayDebug(
        int width,
        int height);


    //-------------------------------------------------------------------------
    // Clear state
    //-------------------------------------------------------------------------

    Vector3 m_clearColor{
        0.0f,
        0.0f,
        0.0f
    };
};