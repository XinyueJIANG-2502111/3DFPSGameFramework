#pragma once

#include <memory>

class Shader;
class IRendererBackend;

class ShaderLoader
{
public:
    explicit ShaderLoader(
        IRendererBackend& rendererBackend);

    std::unique_ptr<Shader> Load(
        const char* vertexShaderPath,
        const char* pixelShaderPath);

private:
    IRendererBackend& m_rendererBackend;
};