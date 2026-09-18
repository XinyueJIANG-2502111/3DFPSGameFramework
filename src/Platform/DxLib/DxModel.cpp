#include "Platform/DxLib/DxModel.h"

#include <DxLib.h>

DxModel::DxModel(const char* filePath)
    : m_handle(MV1LoadModel(filePath))
{
}

DxModel::~DxModel()
{
    if (IsValid())
    {
        MV1DeleteModel(m_handle);
    }
}

DxModel::DxModel(DxModel&& other) noexcept
    : m_handle(other.m_handle)
{
    // EN: The moved-from object must no longer own the handle.
    //     Otherwise both destructors would attempt to release the
    //     same DxLib resource.
    //
    // JP: Move 元 Object は Handle の所有権を失う必要がある。
    //     そうしないと両方の Destructor が同じ DxLib Resource を
    //     解放しようとしてしまう。
    other.m_handle = InvalidHandle;
}

DxModel& DxModel::operator=(DxModel&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    // EN: Release the resource currently owned by this object before
    //     taking ownership from the source object.
    //
    // JP: Move 元から所有権を受け取る前に、この Object が現在
    //     所有している Resource を解放する。
    if (IsValid())
    {
        MV1DeleteModel(m_handle);
    }

    m_handle = other.m_handle;
    other.m_handle = InvalidHandle;

    return *this;
}

bool DxModel::IsValid() const
{
    return m_handle != InvalidHandle;
}

int DxModel::GetHandle() const
{
    return m_handle;
}