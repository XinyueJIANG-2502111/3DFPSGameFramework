#include "Platform/DxLib/DxShaderResource.h"

#include <DxLib.h>

#include <utility>

DxShaderResource::DxShaderResource(
    const char* vertexShaderPath,
    const char* pixelShaderPath)
{
    m_vertexShaderHandle =
        LoadVertexShader(
            vertexShaderPath);

    if (m_vertexShaderHandle == InvalidHandle)
    {
        return;
    }

    m_pixelShaderHandle =
        LoadPixelShader(
            pixelShaderPath);

    if (m_pixelShaderHandle == InvalidHandle)
    {
        DeleteShader(
            m_vertexShaderHandle);

        m_vertexShaderHandle =
            InvalidHandle;

        return;
    }
}

DxShaderResource::~DxShaderResource()
{
    if (m_vertexShaderHandle != InvalidHandle)
    {
        DeleteShader(
            m_vertexShaderHandle);
    }

    if (m_pixelShaderHandle != InvalidHandle)
    {
        DeleteShader(
            m_pixelShaderHandle);
    }
}

DxShaderResource::DxShaderResource(
    DxShaderResource&& other) noexcept
    : m_vertexShaderHandle(
        other.m_vertexShaderHandle)
    , m_pixelShaderHandle(
        other.m_pixelShaderHandle)
{
    other.m_vertexShaderHandle =
        InvalidHandle;

    other.m_pixelShaderHandle =
        InvalidHandle;
}

DxShaderResource&
DxShaderResource::operator=(
    DxShaderResource&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    if (m_vertexShaderHandle != InvalidHandle)
    {
        DeleteShader(
            m_vertexShaderHandle);
    }

    if (m_pixelShaderHandle != InvalidHandle)
    {
        DeleteShader(
            m_pixelShaderHandle);
    }

    m_vertexShaderHandle =
        other.m_vertexShaderHandle;

    m_pixelShaderHandle =
        other.m_pixelShaderHandle;

    other.m_vertexShaderHandle =
        InvalidHandle;

    other.m_pixelShaderHandle =
        InvalidHandle;

    return *this;
}

bool DxShaderResource::IsValid() const
{
    return
        m_vertexShaderHandle != InvalidHandle &&
        m_pixelShaderHandle != InvalidHandle;
}

int DxShaderResource::GetVertexShaderHandle() const
{
    return m_vertexShaderHandle;
}

int DxShaderResource::GetPixelShaderHandle() const
{
    return m_pixelShaderHandle;
}