#include "Engine/Rendering/ModelLoader.h"

#include "Engine/Rendering/Model.h"
#include "Engine/Rendering/Renderer.h"

#include "Engine/Rendering/IModelResource.h"

#include <utility>

ModelLoader::ModelLoader(
    IRendererBackend& rendererBackend)
    : m_rendererBackend(rendererBackend)
{
}

std::unique_ptr<Model> ModelLoader::Load(
    const char* filePath)
{
    auto resource =
        m_rendererBackend.CreateModelResource(
            filePath);

    if (!resource)
    {
        return nullptr;
    }

    return Model::Create(
        std::move(resource));
}