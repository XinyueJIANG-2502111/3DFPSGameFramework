#pragma once

#include <memory>
#include <utility>

#include "Engine/Rendering/IModelResource.h"

class Model::Impl
{
public:
    explicit Impl(
        std::unique_ptr<IModelResource> resource)
        : resource(std::move(resource))
    {
    }

    // EN: Owns the backend-specific model resource while keeping
    //     backend types out of Model's public interface.
    //
    // JP: Backend 固有の型を Model の Public Interface から隠したまま、
    //     Backend Model Resource を所有する。
    std::unique_ptr<IModelResource> resource;
};