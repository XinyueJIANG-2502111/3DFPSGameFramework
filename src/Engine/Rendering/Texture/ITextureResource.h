#pragma once

class ITextureResource
{
public:
    virtual ~ITextureResource() = default;

    ITextureResource(const ITextureResource&) = delete;
    ITextureResource& operator=(const ITextureResource&) = delete;

protected:
    // EN: Backend texture resources are created by the active renderer and
    //     owned through this backend-independent interface.
    //
    // JP: Backend Texture Resource �͌��݂� Renderer �ɂ�����A
    //     Backend �ŗL�̖������ Interface �𒼐ڎ����B
    ITextureResource() = default;
};
