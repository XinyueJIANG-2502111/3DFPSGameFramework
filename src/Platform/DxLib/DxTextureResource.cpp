#include "Platform/DxLib/DxTextureResource.h"

#include <DxLib.h>

DxTextureResource::DxTextureResource(
    const char* filePath)
    : m_graphHandle(
        LoadGraph(filePath, TRUE))
{
    // EN: LoadGraph returns InvalidHandle on failure; keeping that value
    //     makes failed resources safe to destroy and easy to reject.
    //
    // JP: LoadGraph �̎��s���̓n���h���𐶂̂܂܎c���B
    //     ����ɂ���āA������� Resource �����S�ɏ������A�g�p���~�߂�B
}

DxTextureResource::~DxTextureResource()
{
    if (m_graphHandle != InvalidHandle)
    {
        DeleteGraph(m_graphHandle);
    }
}

DxTextureResource::DxTextureResource(
    DxTextureResource&& other) noexcept
    : m_graphHandle(other.m_graphHandle)
{
    other.m_graphHandle = InvalidHandle;
}

DxTextureResource& DxTextureResource::operator=(
    DxTextureResource&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    if (m_graphHandle != InvalidHandle)
    {
        DeleteGraph(m_graphHandle);
    }

    m_graphHandle = other.m_graphHandle;
    other.m_graphHandle = InvalidHandle;

    return *this;
}

bool DxTextureResource::IsValid() const
{
    return m_graphHandle != InvalidHandle;
}

int DxTextureResource::GetGraphHandle() const
{
    return m_graphHandle;
}
