#include "Engine/Scene/SceneManager.h"

#include "Engine/Scene/IScene.h"

#include <utility>

SceneManager::~SceneManager()
{
    Shutdown();
}

void SceneManager::RequestChange(std::unique_ptr<IScene> nextScene)
{
    // EN: A requested scene is stored instead of becoming active immediately.
    //     This guarantees that the current scene cannot be destroyed while
    //     one of its callbacks is still executing.
    //
    // JP: 要求されたシーンは即座に有効化せず、切り替え待ちとして保持する。
    //     これにより、現在のシーンのコールバック実行中に
    //     そのシーン自身が破棄されることを防ぐ。
    m_pendingScene = std::move(nextScene);
}

void SceneManager::ApplyPendingChange()
{
    if (!m_pendingScene)
    {
        return;
    }

    if (m_currentScene)
    {
        m_currentScene->OnExit();
    }

    // EN: Replacing the owning unique_ptr destroys the previous scene here,
    //     giving scene destruction one predictable point in the frame.
    //
    // JP: 所有する unique_ptr をここで置き換えることで旧シーンを破棄し、
    //     フレーム内でのシーン破棄タイミングを一箇所に固定する。
    m_currentScene = std::move(m_pendingScene);

    m_currentScene->OnEnter();
}

void SceneManager::Update(float deltaTime)
{
    if (m_currentScene)
    {
        m_currentScene->Update(deltaTime);
    }
}

void SceneManager::Render()
{
    if (m_currentScene)
    {
        m_currentScene->Render();
    }
}

void SceneManager::Shutdown()
{
    // EN: Give the active scene a final lifecycle notification before
    //     releasing ownership during application shutdown.
    //
    // JP: アプリケーション終了時に所有権を解放する前に、
    //     現在のシーンへ最後のライフサイクル通知を行う。
    if (m_currentScene)
    {
        m_currentScene->OnExit();
        m_currentScene.reset();
    }

    // A pending scene was never activated, so OnExit() must not be called.
    // 待機中のシーンは一度も有効化されていないため、OnExit() は呼び出さない。
    m_pendingScene.reset();
}