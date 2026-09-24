#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "Engine/Rendering/ModelLoader.h"

class Model;
class IRendererBackend;

class ResourceSystem
{
public:
    explicit ResourceSystem(
        IRendererBackend& rendererBackend);

    ResourceSystem(const ResourceSystem&) = delete;
    ResourceSystem& operator=(const ResourceSystem&) = delete;

    // EN: Loads or reuses a Model resource identified by its path.
    //     Multiple users requesting the same path share the same
    //     Model resource while it remains alive.
    //
    // JP: Path によって識別される Model Resource を読み込む、
    //     または既存の Resource を再利用する。
    //     同じ Path を要求する複数の User は、Resource が
    //     生存している間、同じ Model Resource を共有する。
    std::shared_ptr<Model> LoadModel(
        const std::string& filePath);

    // EN: Removes expired cache entries that no longer refer to
    //     live Model resources.
    //
    // JP: 生存中の Model Resource を参照していない
    //     Expired Cache Entry を削除する。
    void RemoveExpired();

    // EN: Clears cache references without directly destroying
    //     resources still owned by users.
    //
    // JP: User がまだ所有している Resource を直接破棄せず、
    //     Cache Reference をクリアする。
    void Clear();

private:
    ModelLoader m_modelLoader;

    std::unordered_map<
        std::string,
        std::weak_ptr<Model>> m_modelCache;

    // EN: Converts an asset path into a stable cache key so that
    //     equivalent paths identify the same runtime resource.
    //
    // JP: 同一 Asset を指す等価な Path が同じ Runtime Resource を
    //     識別できるように、安定した Cache Key へ変換する。
    static std::string NormalizePath(
        const std::string& filePath);
};