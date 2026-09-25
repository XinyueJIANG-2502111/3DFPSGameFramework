#pragma once

#include "Engine/Rendering/Renderer.h"

// forward declaration
class ModelInstance;
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

private:
    int m_spotLightHandle = -1;
};