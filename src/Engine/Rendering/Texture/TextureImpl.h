#pragma once

#include <memory>
#include <utility>

#include "Engine/Rendering/Texture/ITextureResource.h"

class Texture::Impl
{
public:
    explicit Impl(
        std::unique_ptr<ITextureResource> resource)
        : resource(std::move(resource))
    {
    }

    // EN: Keeps the backend texture owned by Texture while hiding its
    //     concrete type from Game and Engine users.
    //
    // JP: Backend Texture �̎��^�� Game �� Engine �̕���J������A
    //     Texture �����L�����`�ŕێ�����B
    std::unique_ptr<ITextureResource> resource;
};
