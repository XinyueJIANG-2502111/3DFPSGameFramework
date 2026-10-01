#include "Engine/Rendering/Shader/ShaderLoader.h"
#include "Engine/Rendering/Shader/IShaderResource.h"
#include "Engine/Rendering/Shader/Shader.h"

#include "Engine/Rendering/Renderer.h"

#include <utility>

ShaderLoader::ShaderLoader(
    IRendererBackend& rendererBackend)
    : m_rendererBackend(rendererBackend)
{
}

std::unique_ptr<Shader> ShaderLoader::Load(
    const char* vertexShaderPath,
    const char* pixelShaderPath)
{
    // EN: Ask the active rendering backend to create the
    //     backend-specific shader resource.
    //
    // JP: 現在の Rendering Backend に、
    //     Backend 固有の Shader Resource の生成を要求する。
    auto resource =
        m_rendererBackend.CreateShaderResource(
            vertexShaderPath,
            pixelShaderPath);

    if (!resource)
    {
        return nullptr;
    }

    // EN: Transfer ownership of the backend resource to Shader.
    //
    // JP: Backend Resource の Ownership を Shader に移譲する。
    return Shader::Create(
        std::move(resource));
}