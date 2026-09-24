#include "Engine/Resources/ResourceSystem.h"

#include "Engine/Rendering/Model.h"

#include <filesystem>
#include <memory>
#include <utility>

ResourceSystem::ResourceSystem(
    IRendererBackend& rendererBackend)
    : m_modelLoader(rendererBackend)
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
}

void ResourceSystem::Clear()
{
    m_modelCache.clear();
}

std::string ResourceSystem::NormalizePath(
    const std::string& filePath)
{
    std::filesystem::path path(filePath);

    return path.lexically_normal().generic_string();
}