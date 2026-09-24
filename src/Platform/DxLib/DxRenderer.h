#pragma once

#include "Engine/Rendering/Renderer.h"

class ModelInstance;
//class IModelResource;

class DxRenderer final
    : public IRendererBackend
{
public:
    void Draw(
        const ModelInstance& instance) override;

    std::unique_ptr<IModelResource> CreateModelResource(
        const char* filePath) override;
};