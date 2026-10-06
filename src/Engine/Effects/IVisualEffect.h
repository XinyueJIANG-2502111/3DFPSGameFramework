#pragma once

class Renderer;

// EN: Effects submit engine render data, keeping DxLib implementation
//     details behind Renderer and avoiding a platform dependency.
// JP: Effect は Engine の描画データだけを提出する。DxLib の実装は
//     Renderer の背後に置き、Platform への依存を避ける。
class IVisualEffect
{
public:
    virtual ~IVisualEffect() = default;
    virtual void Update(float deltaTime) = 0;
    virtual void Render(Renderer& renderer) const = 0;
    [[nodiscard]] virtual bool IsFinished() const = 0;
};
