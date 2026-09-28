#pragma once

#include "Engine/Math/Transform.h"

#include <memory>

class Model;
class Shader;

class ModelInstance
{
public:
    ModelInstance() = default;

    // EN: Associates a shared Model resource with this world instance.
    //
    // JP: 共有 Model Resource をこの World Instance に関連付ける。
    void SetModel(std::shared_ptr<Model> model);

    const std::shared_ptr<Model>& GetModel() const;

    void SetShader(
        std::shared_ptr<Shader> shader);

    const std::shared_ptr<Shader>& GetShader() const;

    Transform& GetTransform();
    const Transform& GetTransform() const;

private:
    std::shared_ptr<Model> m_model;
    std::shared_ptr<Shader> m_shader;
    Transform m_transform;
};