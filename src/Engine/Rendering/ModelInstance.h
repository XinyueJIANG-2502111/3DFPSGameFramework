#pragma once

#include "Engine/Math/Transform.h"

#include <memory>

class Model;

class ModelInstance
{
public:
    ModelInstance() = default;

    // EN: Associates a shared Model resource with this world instance.
    //
    // JP: 共有 Model Resource をこの World Instance に関連付ける。
    void SetModel(std::shared_ptr<Model> model);

    const std::shared_ptr<Model>& GetModel() const;

    Transform& GetTransform();
    const Transform& GetTransform() const;

private:
    std::shared_ptr<Model> m_model;
    Transform m_transform;
};