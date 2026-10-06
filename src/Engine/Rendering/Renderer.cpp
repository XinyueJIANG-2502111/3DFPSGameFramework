#include "Engine/Rendering/Renderer.h"

#include "Engine/Math/Vector3.h"

#include "Engine/Rendering/Model/ModelInstance.h"

#include "Engine/Rendering/Fog/FogSettings.h"

#include "Engine/Rendering/Lighting/AmbientLight.h"
#include "Engine/Rendering/Lighting/PointLight.h"
#include "Engine/Rendering/Lighting/SpotLight.h"

#include <algorithm>


//=============================================================================
// Construction
//=============================================================================

Renderer::Renderer(
    IRendererBackend& backend)
    : m_backend(backend)
{
}


//=============================================================================
// Render passes
//=============================================================================

void Renderer::BeginSceneRender(
    int width,
    int height)
{
    m_backend.BeginSceneRender(
        width,
        height);
}

void Renderer::EndSceneRender(
    int width,
    int height)
{
    m_backend.EndSceneRender(
        width,
        height);
}

void Renderer::BeginSceneDepthRender(
    int width,
    int height)
{
    m_backend.BeginSceneDepthRender(
        width,
        height);
}

void Renderer::EndSceneDepthRender()
{
    m_backend.EndSceneDepthRender();
}


//=============================================================================
// Camera shader data
//=============================================================================

void Renderer::SetCameraPosition(
    const Vector3& position)
{
    m_backend.SetCameraPosition(
        position);
}

void Renderer::SetCameraForward(
    const Vector3& forward)
{
    m_backend.SetCameraForward(
        forward);
}

void Renderer::SetCameraRight(
    const Vector3& right)
{
    m_backend.SetCameraRight(
        right);
}

void Renderer::SetCameraUp(
    const Vector3& up)
{
    m_backend.SetCameraUp(
        up);
}

void Renderer::SetCameraFieldOfView(
    float verticalFovRadians)
{
    // EN: Forward the camera's vertical field of view to the backend.
    //     The render-target aspect ratio is managed separately by the
    //     backend because it belongs to the active render target.
    //
    // JP: Camera の Vertical FOV を Backend に渡す。
    //     Render Target の Aspect Ratio は Camera 固有の値ではないため、
    //     Active Render Target を管理する Backend 側で別途管理する。
    m_backend.SetCameraFieldOfView(
        verticalFovRadians);
}


//=============================================================================
// Scene rendering state
//=============================================================================

void Renderer::SetClearColor(
    const Vector3& color)
{
    m_backend.SetClearColor(
        color);
}

void Renderer::SetFog(
    const FogSettings& settings)
{
    FogSettings sanitized =
        settings;

    // EN: Keep normalized fog color inside the valid RGB range.
    //
    // JP: Fog Color を有効な正規化 RGB 範囲内に制限する。
    sanitized.color.x =
        std::clamp(
            sanitized.color.x,
            0.0f,
            1.0f);

    sanitized.color.y =
        std::clamp(
            sanitized.color.y,
            0.0f,
            1.0f);

    sanitized.color.z =
        std::clamp(
            sanitized.color.z,
            0.0f,
            1.0f);

    // EN: Fog distances cannot be negative.
    //
    // JP: Fog の距離に負の値を許可しない。
    sanitized.startDistance =
        std::max(
            sanitized.startDistance,
            0.0f);

    sanitized.endDistance =
        std::max(
            sanitized.endDistance,
            0.0f);

    // EN: Linear fog requires a non-zero distance range.
    //
    // JP: Linear Fog では 0 ではない Distance Range が必要になる。
    constexpr float minimumFogRange =
        0.01f;

    sanitized.endDistance =
        std::max(
            sanitized.endDistance,
            sanitized.startDistance +
            minimumFogRange);

    m_backend.SetFog(
        sanitized);
}

void Renderer::SetAmbientLight(
    const AmbientLight& light)
{
    m_backend.SetAmbientLight(
        light);
}

void Renderer::SetPointLight(
    const PointLight& light)
{
    m_backend.SetPointLight(
        light);
}

void Renderer::SetSpotLight(
    const SpotLight& light)
{
    m_backend.SetSpotLight(
        light);
}


//=============================================================================
// Model drawing
//=============================================================================

void Renderer::Draw(
    const ModelInstance& instance)
{
    m_backend.Draw(
        instance);
}

void Renderer::Draw(
    const ModelInstance& instance,
    const Shader& shader)
{
    m_backend.Draw(
        instance,
        shader);
}


//=============================================================================
// Volumetric rendering
//=============================================================================

void Renderer::SetVolumetricSettings(
    const ShaderVolumetricData& settings)
{
    m_backend.SetVolumetricSettings(
        settings);
}

void Renderer::DrawVolumetricCone(
    const VolumetricCone& cone,
    const Shader& shader)
{
    m_backend.DrawVolumetricCone(
        cone,
        shader);
}