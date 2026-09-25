#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/ModelInstance.h"

#include "Engine/Rendering/FogSettings.h"
#include "Engine/Math/Vector3.h"

#include "Engine/Rendering/Lighting/PointLight.h"
#include "Engine/Rendering/Lighting/SpotLight.h"
#include "Engine/Rendering/Lighting/AmbientLight.h"

#include <algorithm>

Renderer::Renderer(
    IRendererBackend& backend)
    : m_backend(backend)
{
}

void Renderer::Draw(
    const ModelInstance& instance)
{
    m_backend.Draw(instance);
}

void Renderer::SetClearColor(
    const Vector3& color)
{
    m_backend.SetClearColor(color);
}

void Renderer::SetFog(
    const FogSettings& settings)
{
    FogSettings sanitized = settings;

    // EN: Keep normalized fog color inside the valid RGB range.
    //
    // JP: Fog Color を有効な正規化 RGB 範囲内に制限する。
    sanitized.color.x =
        std::clamp(sanitized.color.x, 0.0f, 1.0f);

    sanitized.color.y =
        std::clamp(sanitized.color.y, 0.0f, 1.0f);

    sanitized.color.z =
        std::clamp(sanitized.color.z, 0.0f, 1.0f);

    // EN: Fog distances cannot be negative.
    //
    // JP: Fog の距離に負の値を許可しない。
    sanitized.startDistance =
        std::max(sanitized.startDistance, 0.0f);

    sanitized.endDistance =
        std::max(sanitized.endDistance, 0.0f);

    // EN: Linear fog requires the end distance to be greater
    //     than the start distance.
    //
    // JP: Linear Fog では End Distance が
    //     Start Distance より大きい必要がある。
    constexpr float minimumFogRange = 0.01f;

    sanitized.endDistance =
        std::max(
            sanitized.endDistance,
            sanitized.startDistance + minimumFogRange);

    m_backend.SetFog(sanitized);
}

void Renderer::SetPointLight(
    const PointLight& light)
{
    m_backend.SetPointLight(light);
}

void Renderer::SetSpotLight(
    const SpotLight& light)
{
    m_backend.SetSpotLight(light);
}

void Renderer::SetAmbientLight(
    const AmbientLight& light)
{
    m_backend.SetAmbientLight(light);
}