#pragma once

class Shader;
class IShaderResource;

class ShaderResourceAccess
{
public:
    // EN: Provides rendering implementations with controlled access
    //     to the backend shader resource owned by Shader.
    //
    // JP: Rendering 実装が Shader の所有する Backend Resource へ
    //     制御された形でアクセスするための内部境界を提供する。
    static const IShaderResource* Get(
        const Shader& shader);
};