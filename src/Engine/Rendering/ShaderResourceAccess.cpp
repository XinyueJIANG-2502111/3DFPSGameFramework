#include "Engine/Rendering/ShaderResourceAccess.h"

#include "Engine/Rendering/Shader.h"
#include "Engine/Rendering/ShaderImpl.h"

const IShaderResource* ShaderResourceAccess::Get(
    const Shader& shader)
{
    if (shader.m_impl == nullptr)
    {
        return nullptr;
    }

    return shader.m_impl->resource.get();
}