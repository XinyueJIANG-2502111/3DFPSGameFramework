#pragma once

#include <memory>

class IScene;

class SceneManager
{
public:
    SceneManager() = default;
    ~SceneManager();

    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    SceneManager(SceneManager&&) = delete;
    SceneManager& operator=(SceneManager&&) = delete;

    void RequestChange(std::unique_ptr<IScene> nextScene);

    void ApplyPendingChange();

    void Update(float deltaTime);
    void Render();

    void Shutdown();

private:
    // EN: SceneManager has exclusive ownership of both the active scene
    //     and a scene waiting to become active. unique_ptr makes that
    //     ownership and destruction point explicit.
    //
    // JP: SceneManager は現在のシーンと切り替え待ちのシーンを排他的に所有する。
    //     unique_ptr によって所有権と破棄タイミングを明確にする。
    std::unique_ptr<IScene> m_currentScene;
    std::unique_ptr<IScene> m_pendingScene;
};