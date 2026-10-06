#pragma once

#include <memory>


//=============================================================================
// Forward declarations
//=============================================================================

class IModelResource;
class IShaderResource;
class ModelInstance;
class Shader;

struct Vector3;
struct FogSettings;
struct AmbientLight;
struct PointLight;
struct SpotLight;
struct VolumetricCone;
struct ShaderVolumetricData;


//=============================================================================
// IRendererBackend
//=============================================================================

class IRendererBackend
{
public:
    virtual ~IRendererBackend() = default;


    //-------------------------------------------------------------------------
    // Resource creation
    //-------------------------------------------------------------------------

    // EN: Creates a backend-specific model resource.
    //
    // JP: Backend 固有の Model Resource を生成する。
    virtual std::unique_ptr<IModelResource> CreateModelResource(
        const char* filePath) = 0;

    // EN: Creates a backend-specific shader resource.
    //
    // JP: Backend 固有の Shader Resource を生成する。
    virtual std::unique_ptr<IShaderResource> CreateShaderResource(
        const char* vertexShaderPath,
        const char* pixelShaderPath) = 0;


    //-------------------------------------------------------------------------
    // Frame / render passes
    //-------------------------------------------------------------------------

    virtual void BeginSceneRender(
        int width,
        int height) = 0;

    virtual void EndSceneRender(
        int width,
        int height) = 0;

    virtual void BeginSceneDepthRender(
        int width,
        int height) = 0;

    virtual void EndSceneDepthRender() = 0;


    //-------------------------------------------------------------------------
    // Camera
    //-------------------------------------------------------------------------

    virtual void SetCameraPosition(
        const Vector3& position) = 0;

    virtual void SetCameraForward(
        const Vector3& forward) = 0;

    virtual void SetCameraFieldOfView(
        float verticalFovRadians) = 0;

    virtual void SetCameraRight(
        const Vector3& right) = 0;

    virtual void SetCameraUp(
        const Vector3& up) = 0;


    //-------------------------------------------------------------------------
    // Scene rendering state
    //-------------------------------------------------------------------------

    virtual void SetClearColor(
        const Vector3& color) = 0;

    virtual void SetFog(
        const FogSettings& settings) = 0;

    virtual void SetAmbientLight(
        const AmbientLight& light) = 0;

    virtual void SetPointLight(
        const PointLight& light) = 0;

    virtual void SetSpotLight(
        const SpotLight& light) = 0;


    //-------------------------------------------------------------------------
    // Model drawing
    //-------------------------------------------------------------------------

    virtual void Draw(
        const ModelInstance& instance) = 0;

    virtual void Draw(
        const ModelInstance& instance,
        const Shader& shader) = 0;


    //-------------------------------------------------------------------------
    // Volumetric rendering
    //-------------------------------------------------------------------------

    virtual void SetVolumetricSettings(
        const ShaderVolumetricData& settings) = 0;

    virtual void DrawVolumetricCone(
        const VolumetricCone& cone,
        const Shader& shader) = 0;


    //-------------------------------------------------------------------------
    // Lifecycle
    //-------------------------------------------------------------------------

    // EN: Releases backend-owned rendering resources before
    //     the graphics platform is shut down.
    //
    // JP: Graphics Platform が終了する前に、
    //     Backend 所有の Rendering Resource を解放する。
    virtual void Shutdown() = 0;
};

class Renderer
{
public:
    explicit Renderer(
        IRendererBackend& backend);


    //-------------------------------------------------------------------------
    // Render passes
    //-------------------------------------------------------------------------

    void BeginSceneRender(
        int width,
        int height);

    void EndSceneRender(
        int width,
        int height);

    void BeginSceneDepthRender(
        int width,
        int height);

    void EndSceneDepthRender();


    //-------------------------------------------------------------------------
    // Camera shader data
    //-------------------------------------------------------------------------

    void SetCameraPosition(
        const Vector3& position);

    void SetCameraForward(
        const Vector3& forward);

    void SetCameraRight(
        const Vector3& right);

    void SetCameraUp(
        const Vector3& up);

    void SetCameraFieldOfView(
        float verticalFovRadians);

    
    //-------------------------------------------------------------------------
    // Scene rendering state
    //-------------------------------------------------------------------------

    void SetClearColor(
        const Vector3& color);

    void SetFog(
        const FogSettings& settings);

    void SetAmbientLight(
        const AmbientLight& light);

    void SetPointLight(
        const PointLight& light);

    void SetSpotLight(
        const SpotLight& light);


    //-------------------------------------------------------------------------
    // Model drawing
    //-------------------------------------------------------------------------

    // EN: Draws a model instance using its configured rendering state.
    //
    // JP: Model Instance に設定された Rendering State を使用して描画する。
    void Draw(
        const ModelInstance& instance);

    // EN: Draws a model instance using an explicitly supplied shader
    //     without changing the shader stored by the instance.
    //
    // JP: Model Instance が保持する Shader を変更せず、
    //     明示的に指定された Shader を使用して描画する。
    void Draw(
        const ModelInstance& instance,
        const Shader& shader);


    //-------------------------------------------------------------------------
    // Volumetric rendering
    //-------------------------------------------------------------------------

    void SetVolumetricSettings(
        const ShaderVolumetricData& settings);

    void DrawVolumetricCone(
        const VolumetricCone& cone,
        const Shader& shader);


private:
    IRendererBackend& m_backend;
};