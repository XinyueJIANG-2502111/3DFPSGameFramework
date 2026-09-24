#include "Engine/Rendering/Model.h"
#include "Engine/Rendering/ModelImpl.h"

#include "Engine/Rendering/IModelResource.h"

#include <memory>
#include <utility>

Model::Model(std::unique_ptr<Impl> impl)
    : m_impl(std::move(impl))
{
}

Model::~Model() = default;

Model::Model(Model&& other) noexcept = default;

Model& Model::operator=(Model&& other) noexcept = default;

bool Model::IsValid() const
{
    return
        (m_impl != nullptr) &&
        (m_impl->resource != nullptr);
}

std::unique_ptr<Model> Model::Create(
    std::unique_ptr<IModelResource> resource)
{
    if (!resource)
    {
        return nullptr;
    }

    auto impl =
        std::make_unique<Impl>(
            std::move(resource));

    return std::unique_ptr<Model>(
        new Model(std::move(impl)));
}