#include "Engine/Rendering/Texture/TextureLoader.h"

#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/Texture/ITextureResource.h"
#include "Engine/Rendering/Texture/Texture.h"

#include <utility>

TextureLoader::TextureLoader(
    IRendererBackend& rendererBackend)
    : m_rendererBackend(rendererBackend)
{
}

std::unique_ptr<Texture> TextureLoader::Load(
    const char* filePath)
{
    auto resource =
        m_rendererBackend.CreateTextureResource(
            filePath);

    if (!resource)
    {
        return nullptr;
    }

    return Texture::Create(
        std::move(resource));
}
