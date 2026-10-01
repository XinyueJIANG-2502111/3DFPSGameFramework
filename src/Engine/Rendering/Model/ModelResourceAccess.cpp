#include "Engine/Rendering/Model/ModelResourceAccess.h"

#include "Engine/Rendering/Model/Model.h"
#include "Engine/Rendering/Model/ModelImpl.h"
#include "Engine/Rendering/Model/IModelResource.h"

const IModelResource* ModelResourceAccess::Get(
    const Model& model)
{
    if (model.m_impl == nullptr)
    {
        return nullptr;
    }

    return model.m_impl->resource.get();
}