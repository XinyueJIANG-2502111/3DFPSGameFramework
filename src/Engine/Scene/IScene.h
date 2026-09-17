#pragma once

class IScene
{
public:
    virtual ~IScene() = default;

    // EN: Optional lifecycle hooks for work that must happen specifically
    //     when a scene becomes active or stops being active.
    //
    // JP: シーンが有効になった時、または無効になる時にだけ必要な処理を
    //     実装するための任意のライフサイクルフック。
    virtual void OnEnter() {}
    virtual void OnExit() {}

    // EN: Update and Render define the essential runtime behavior of a scene,
    //     so every concrete scene must provide these operations.
    //
    // JP: Update と Render はシーン実行時の基本動作を定義するため、
    //     すべての具体的なシーンが実装する必要がある。
    virtual void Update(float deltaTime) = 0;
    virtual void Render() = 0;
};