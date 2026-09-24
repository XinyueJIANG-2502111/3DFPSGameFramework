#include "Platform/DxLib/DxRenderer.h"

#include "Engine/Rendering/IModelResource.h"
#include "Engine/Rendering/Model.h"
#include "Engine/Rendering/ModelInstance.h"
#include "Engine/Rendering/ModelResourceAccess.h"
#include "Engine/Math/Matrix4.h"
#include "Platform/DxLib/DxModelResource.h"

#include <DxLib.h>

namespace
{
    MATRIX ToDxMatrix(const Matrix4& matrix)
    {
        MATRIX result{};

        // EN: Engine matrices use column-vector semantics.
        //     Transpose here when crossing the DxLib backend boundary.
        //
        // JP: Engine の Matrix は列ベクトル規約を使用する。
        //     DxLib Backend 境界を越える際にここで転置する。
        for (int row = 0; row < 4; ++row)
        {
            for (int column = 0; column < 4; ++column)
            {
                result.m[row][column] =
                    matrix.m[column][row];
            }
        }

        return result;
    }
}

void DxRenderer::Draw(
    const ModelInstance& instance)
{
    const std::shared_ptr<Model>& model =
        instance.GetModel();

    if (!model || !model->IsValid())
    {
        return;
    }

    const IModelResource* resource =
        ModelResourceAccess::Get(*model);

    if (resource == nullptr)
    {
        return;
    }

    const auto* dxResource =
        dynamic_cast<const DxModelResource*>(
            resource);

    if (dxResource == nullptr)
    {
        return;
    }

    const int handle =
        dxResource->GetModel().GetHandle();

    const Matrix4 worldMatrix =
        instance.GetTransform().ToMatrix();

    const MATRIX dxWorldMatrix =
        ToDxMatrix(worldMatrix);

    MV1SetMatrix(
        handle,
        dxWorldMatrix);

    MV1DrawModel(handle);
}

std::unique_ptr<IModelResource>
DxRenderer::CreateModelResource(
    const char* filePath)
{
    auto resource =
        std::make_unique<DxModelResource>(
            filePath);

    if (!resource->IsValid())
    {
        return nullptr;
    }

    return resource;
}