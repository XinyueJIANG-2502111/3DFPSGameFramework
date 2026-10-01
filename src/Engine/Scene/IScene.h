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

    // EN: Renders only opaque geometry that contributes to
    //     scene depth and volumetric occlusion.
    //
    // JP: Scene Depth と Volumetric Occlusion に寄与する
    //     Opaque Geometry のみを描画する。
    // EN: Scenes without opaque depth contributors may leave this pass empty.
    // JP: 深度へ寄与する不透明物体がないシーンでは、このパスを省略できる。
    virtual void RenderDepth() {}
};