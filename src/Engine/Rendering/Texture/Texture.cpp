#include "Engine/Rendering/Texture/Texture.h"

#include "Engine/Rendering/Texture/ITextureResource.h"
#include "Engine/Rendering/Texture/TextureImpl.h"

#include <memory>
#include <utility>

Texture::Texture(std::unique_ptr<Impl> impl)
    : m_impl(std::move(impl))
{
}

Texture::~Texture() = default;

Texture::Texture(Texture&& other) noexcept = default;

Texture& Texture::operator=(Texture&& other) noexcept = default;

bool Texture::IsValid() const
{
    return
        m_impl != nullptr &&
        m_impl->resource != nullptr;
}

std::unique_ptr<Texture> Texture::Create(
    std::unique_ptr<ITextureResource> resource)
{
    if (!resource)
    {
        return nullptr;
    }

    auto impl =
        std::make_unique<Impl>(
            std::move(resource));

    return std::unique_ptr<Texture>(
        new Texture(std::move(impl)));
}
