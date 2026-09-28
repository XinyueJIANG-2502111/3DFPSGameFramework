#include "Engine/Resources/ResourceSystem.h"

#include "Engine/Rendering/Model.h"
#include "Engine/Rendering/Shader.h"

#include <filesystem>
#include <memory>
#include <utility>

ResourceSystem::ResourceSystem(
    IRendererBackend& rendererBackend)
    : m_modelLoader(rendererBackend)
    , m_shaderLoader(rendererBackend)
{
}

std::shared_ptr<Model> ResourceSystem::LoadModel(
    const std::string& filePath)
{
    const std::string key =
        NormalizePath(filePath);

    const auto iterator =
        m_modelCache.find(key);

    if (iterator != m_modelCache.end())
    {
        if (std::shared_ptr<Model> existing =
            iterator->second.lock())
        {
            return existing;
        }
    }

    // EN: ModelLoader uses the configured rendering backend to create
    //     the backend-specific model resource.
    //
    // JP: ModelLoader は設定された Rendering Backend を使用して、
    //     Backend 固有の Model Resource を生成する。
    std::unique_ptr<Model> loaded =
        m_modelLoader.Load(key.c_str());

    if (!loaded)
    {
        return nullptr;
    }

    std::shared_ptr<Model> resource =
        std::move(loaded);

    m_modelCache[key] = resource;

    return resource;
}

std::shared_ptr<Shader> ResourceSystem::LoadShader(
    const std::string& vertexShaderPath,
    const std::string& pixelShaderPath)
{
    const std::string normalizedVertexPath =
        NormalizePath(vertexShaderPath);

    const std::string normalizedPixelPath =
        NormalizePath(pixelShaderPath);

    // EN: A shader resource is identified by both its vertex
    //     and pixel shader paths.
    //
    // JP: Shader Resource は Vertex Shader と Pixel Shader、
    //     両方の Path の組み合わせによって識別する。
    const std::string key =
        normalizedVertexPath +
        "|" +
        normalizedPixelPath;

    const auto iterator =
        m_shaderCache.find(key);

    if (iterator != m_shaderCache.end())
    {
        if (std::shared_ptr<Shader> existing =
            iterator->second.lock())
        {
            return existing;
        }
    }

    std::unique_ptr<Shader> loaded =
        m_shaderLoader.Load(
            normalizedVertexPath.c_str(),
            normalizedPixelPath.c_str());

    if (!loaded)
    {
        return nullptr;
    }

    std::shared_ptr<Shader> resource =
        std::move(loaded);

    m_shaderCache[key] = resource;

    return resource;
}

void ResourceSystem::RemoveExpired()
{
    for (auto iterator = m_modelCache.begin();
        iterator != m_modelCache.end();)
    {
        if (iterator->second.expired())
        {
            iterator =
                m_modelCache.erase(iterator);
        }
        else
        {
            ++iterator;
        }
    }

    for (auto iterator = m_shaderCache.begin();
        iterator != m_shaderCache.end();)
    {
        if (iterator->second.expired())
        {
            iterator =
                m_shaderCache.erase(iterator);
        }
        else
        {
            ++iterator;
        }
    }
}

void ResourceSystem::Clear()
{
    m_modelCache.clear();
    m_shaderCache.clear();
}

std::string ResourceSystem::NormalizePath(
    const std::string& filePath)
{
    std::filesystem::path path(filePath);

    return path.lexically_normal().generic_string();
}