#pragma once

#include <memory>

// forward declaration
class IModelResource;
class ModelInstance;
class IShaderResource;
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

    // EN: Creates a backend-specific shader resource.
    //
    // JP: Backend 固有の Shader Resource を生成する。
    virtual std::unique_ptr<IShaderResource> CreateShaderResource(
        const char* vertexShaderPath,
        const char* pixelShaderPath) = 0;

    virtual void SetCameraPosition(
        const Vector3& position) = 0;

    // EN: Releases backend-owned rendering resources before
    //     the graphics platform is shut down.
    //
    // JP: Graphics Platform が終了する前に、Backend が所有する
    //     Rendering Resource を解放する。
    virtual void Shutdown() = 0;
};

class Renderer
{
public:
    explicit Renderer(
        IRendererBackend& backend);

    // EN: Draws one model instance using its resource and transform.
    //
    // JP: Model Instance が保持する Resource と Transform を使用して
    //     1 つの Model Instance を描画する。
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

    void SetCameraPosition(
        const Vector3& position);

private:
    IRendererBackend& m_backend;
};