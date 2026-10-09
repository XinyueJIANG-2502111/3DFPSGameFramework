#pragma once

#include <memory>

class ITextureResource;
class TextureLoader;
class TextureResourceAccess;

class Texture
{
public:
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&&) noexcept;
    Texture& operator=(Texture&&) noexcept;

    [[nodiscard]] bool IsValid() const;

private:
    class Impl;

    static std::unique_ptr<Texture> Create(
        std::unique_ptr<ITextureResource> resource);

    explicit Texture(std::unique_ptr<Impl> impl);

    std::unique_ptr<Impl> m_impl;

    friend class TextureLoader;
    friend class TextureResourceAccess;
};
