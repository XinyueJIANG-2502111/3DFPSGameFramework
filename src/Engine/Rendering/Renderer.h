#pragma once

#include <memory>

class ModelInstance;
class IModelResource;

class IRendererBackend
{
public:
    virtual ~IRendererBackend() = default;

    virtual std::unique_ptr<IModelResource> CreateModelResource(
        const char* filePath) = 0;

    virtual void Draw(
        const ModelInstance& instance) = 0;
};

class Renderer
{
public:
    explicit Renderer(
        IRendererBackend& backend);

    // EN: Draws one model instance using its resource and transform.
    //
    // JP: Model Instance ‚ª•Û‚·‚é Resource ‚Æ Transform ‚ğg—p‚µ‚Ä
    //     1 ‚Â‚Ì Model Instance ‚ğ•`‰æ‚·‚éB
    void Draw(
        const ModelInstance& instance);

private:
    IRendererBackend& m_backend;
};