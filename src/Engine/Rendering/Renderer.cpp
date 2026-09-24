#include "Engine/Rendering/Renderer.h"

#include "Engine/Rendering/ModelInstance.h"

Renderer::Renderer(
    IRendererBackend& backend)
    : m_backend(backend)
{
}

void Renderer::Draw(
    const ModelInstance& instance)
{
    m_backend.Draw(instance);
}