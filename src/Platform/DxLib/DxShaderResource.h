#pragma once

#include "Engine/Rendering/Shader/IShaderResource.h"

class DxShaderResource final
    : public IShaderResource
{
public:
    static constexpr int InvalidHandle = -1;

    DxShaderResource(
        const char* vertexShaderPath,
        const char* pixelShaderPath);

    ~DxShaderResource();

    DxShaderResource(
        const DxShaderResource&) = delete;

    DxShaderResource& operator=(
        const DxShaderResource&) = delete;

    DxShaderResource(
        DxShaderResource&& other) noexcept;

    DxShaderResource& operator=(
        DxShaderResource&& other) noexcept;

    bool IsValid() const;

    int GetVertexShaderHandle() const;
    int GetPixelShaderHandle() const;

private:
    int m_vertexShaderHandle =
        InvalidHandle;

    int m_pixelShaderHandle =
        InvalidHandle;
};