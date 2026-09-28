#pragma once

#include <memory>
#include <utility>

#include "Engine/Rendering/IShaderResource.h"

class Shader::Impl
{
public:
    explicit Impl(
        std::unique_ptr<IShaderResource> resource)
        : resource(std::move(resource))
    {
    }

    // EN: Owns the backend-specific shader resource without
    //     exposing backend types through the public Shader API.
    //
    // JP: Backend 固有型を Public Shader API に公開せず、
    //     Backend Shader Resource を所有する。
    std::unique_ptr<IShaderResource> resource;
};