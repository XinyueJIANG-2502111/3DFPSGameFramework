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

void ModelInstance::SetShader(
    std::shared_ptr<Shader> shader)
{
    m_shader = std::move(shader);
}

const std::shared_ptr<Shader>& ModelInstance::GetShader() const
{
    return m_shader;
}

Transform& ModelInstance::GetTransform()
{
    return m_transform;
}

const Transform& ModelInstance::GetTransform() const
{
    return m_transform;
}