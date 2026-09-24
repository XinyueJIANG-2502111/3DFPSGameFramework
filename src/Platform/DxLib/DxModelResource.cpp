#include "Platform/DxLib/DxModelResource.h"

DxModelResource::DxModelResource(
    const char* filePath)
    : m_model(filePath)
{
}

bool DxModelResource::IsValid() const
{
    return m_model.IsValid();
}

DxModel& DxModelResource::GetModel()
{
    return m_model;
}

const DxModel& DxModelResource::GetModel() const
{
    return m_model;
}