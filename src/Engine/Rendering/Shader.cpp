#include "Engine/Rendering/Shader.h"

#include "Engine/Rendering/ShaderImpl.h"

#include <utility>

Shader::Shader(
    std::unique_ptr<Impl> impl)
    : m_impl(std::move(impl))
{
}

Shader::~Shader() = default;

Shader::Shader(
    Shader&&) noexcept = default;

Shader& Shader::operator=(
    Shader&&) noexcept = default;

bool Shader::IsValid() const
{
    return
        m_impl != nullptr &&
        m_impl->resource != nullptr;
}

std::unique_ptr<Shader> Shader::Create(
    std::unique_ptr<IShaderResource> resource)
{
    if (!resource)
    {
        return nullptr;
    }

    auto impl =
        std::make_unique<Impl>(
            std::move(resource));

    return std::unique_ptr<Shader>(
        new Shader(std::move(impl)));
}