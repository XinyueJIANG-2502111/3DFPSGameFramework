#include "Engine/Rendering/Shader/ShaderResourceAccess.h"

#include "Engine/Rendering/Shader/Shader.h"
#include "Engine/Rendering/Shader/ShaderImpl.h"

const IShaderResource* ShaderResourceAccess::Get(
    const Shader& shader)
{
    if (shader.m_impl == nullptr)
    {
        return nullptr;
    }

    return shader.m_impl->resource.get();
}