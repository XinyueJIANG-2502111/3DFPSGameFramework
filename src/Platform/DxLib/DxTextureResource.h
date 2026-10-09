#pragma once

#include "Engine/Rendering/Texture/ITextureResource.h"

class DxTextureResource final
    : public ITextureResource
{
public:
    static constexpr int InvalidHandle = -1;

    explicit DxTextureResource(
        const char* filePath);

    ~DxTextureResource();

    DxTextureResource(const DxTextureResource&) = delete;
    DxTextureResource& operator=(const DxTextureResource&) = delete;

    DxTextureResource(DxTextureResource&& other) noexcept;
    DxTextureResource& operator=(DxTextureResource&& other) noexcept;

    [[nodiscard]] bool IsValid() const;

    // EN: Native handle access is limited to the DxLib backend boundary.
    //
    // JP: Native Handle �ւ̎����́A DxLib Backend ���E���Ɍ��肷��B
    int GetGraphHandle() const;

private:
    int m_graphHandle = InvalidHandle;
};
