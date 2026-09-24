#include "Engine/Rendering/ModelInstance.h"

#include "Engine/Rendering/Model.h"

#include <utility>

void ModelInstance::SetModel(
    std::shared_ptr<Model> model)
{
    m_model = std::move(model);
}

const std::shared_ptr<Model>&
ModelInstance::GetModel() const
{
    return m_model;
}

Transform& ModelInstance::GetTransform()
{
    return m_transform;
}

const Transform& ModelInstance::GetTransform() const
{
    return m_transform;
}