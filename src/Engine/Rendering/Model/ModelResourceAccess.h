#pragma once

class Model;
class IModelResource;

class ModelResourceAccess
{
public:
    // EN: Provides rendering implementations with controlled access
    //     to the backend resource owned by Model.
    //
    // JP: Rendering 実装が Model の所有する Backend Resource へ
    //     制御された形でアクセスするための内部境界を提供する。
    static const IModelResource* Get(
        const Model& model);
};