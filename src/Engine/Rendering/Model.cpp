#include "Engine/Rendering/Model.h"

#include "Platform/DxLib/DxModel.h"

#include <memory>
#include <utility>

class Model::Impl
{
public:
    explicit Impl(DxModel&& dxModel)
        : model(std::move(dxModel))
    {
    }

    DxModel model;
};

Model::Model(std::unique_ptr<Impl> impl)
    : m_impl(std::move(impl))
{
}

Model::~Model() = default;

Model::Model(Model&& other) noexcept = default;

Model& Model::operator=(Model&& other) noexcept = default;

bool Model::IsValid() const
{
    return m_impl != nullptr &&
        m_impl->model.IsValid();
}

std::unique_ptr<Model> Model::Load(
    const char* filePath)
{
    DxModel dxModel(filePath);

    if (!dxModel.IsValid())
    {
        return nullptr;
    }

    auto impl =
        std::make_unique<Impl>(
            std::move(dxModel));

    return std::unique_ptr<Model>(
        new Model(std::move(impl)));
}