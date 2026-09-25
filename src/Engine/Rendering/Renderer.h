#pragma once

#include <memory>

// forward declaration
class IModelResource;
class ModelInstance;
struct Vector3;
struct FogSettings;
struct AmbientLight;
struct PointLight;
struct SpotLight;

class IRendererBackend
{
public:
    virtual ~IRendererBackend() = default;

    virtual std::unique_ptr<IModelResource> CreateModelResource(
        const char* filePath) = 0;

    virtual void Draw(
        const ModelInstance& instance) = 0;

    virtual void SetFog(
        const FogSettings& settings) = 0;

    virtual void SetClearColor(
        const Vector3& color) = 0;

    virtual void SetPointLight(
        const PointLight& light) = 0;

    virtual void SetSpotLight(
        const SpotLight& light) = 0;

    virtual void SetAmbientLight(
        const AmbientLight& light) = 0;
};

class Renderer
{
public:
    explicit Renderer(
        IRendererBackend& backend);

    // EN: Draws one model instance using its resource and transform.
    //
    // JP: Model Instance Ç™ï€éùÇ∑ÇÈ Resource Ç∆ Transform ÇégópÇµÇƒ
    //     1 Ç¬ÇÃ Model Instance Çï`âÊÇ∑ÇÈÅB
    void Draw(
        const ModelInstance& instance);

    void SetFog(
        const FogSettings& settings);

    void SetClearColor(
        const Vector3& color);

    void SetPointLight(
        const PointLight& light);

    void SetSpotLight(
        const SpotLight& light);

    void SetAmbientLight(
        const AmbientLight& light);

private:
    IRendererBackend& m_backend;
};