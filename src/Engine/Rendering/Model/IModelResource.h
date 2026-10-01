#pragma once

class IModelResource
{
public:
    virtual ~IModelResource() = default;

    IModelResource(const IModelResource&) = delete;
    IModelResource& operator=(const IModelResource&) = delete;

protected:
    // EN: Backend model resources are created only by concrete
    //     rendering backends and owned through this interface.
    //
    // JP: Backend Model Resource は具体的な Rendering Backend によって
    //     生成され、この Interface を通して所有される。
    IModelResource() = default;
};