#include "Engine/Rendering/Texture/TextureResourceAccess.h"

#include "Engine/Rendering/Texture/Texture.h"
#include "Engine/Rendering/Texture/TextureImpl.h"

const ITextureResource* TextureResourceAccess::Get(
    const Texture& texture)
{
    if (texture.m_impl == nullptr)
    {
        return nullptr;
    }

    return texture.m_impl->resource.get();
}
