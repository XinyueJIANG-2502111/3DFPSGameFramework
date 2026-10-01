#pragma once

#include <memory>

class IShaderResource;
class ShaderLoader;
class ShaderResourceAccess;

class Shader
{
public:
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&&) noexcept;
    Shader& operator=(Shader&&) noexcept;

    bool IsValid() const;

private:
    class Impl;

    static std::unique_ptr<Shader> Create(
        std::unique_ptr<IShaderResource> resource);

    explicit Shader(
        std::unique_ptr<Impl> impl);

    std::unique_ptr<Impl> m_impl;

    friend class ShaderLoader;
    friend class ShaderResourceAccess;
};