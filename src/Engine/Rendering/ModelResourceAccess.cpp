#include "Engine/Rendering/ModelResourceAccess.h"

#include "Engine/Rendering/Model.h"
#include "Engine/Rendering/ModelImpl.h"
#include "Engine/Rendering/IModelResource.h"

const IModelResource* ModelResourceAccess::Get(
    const Model& model)
{
    if (model.m_impl == nullptr)
    {
        return nullptr;
    }

    return model.m_impl->resource.get();
}