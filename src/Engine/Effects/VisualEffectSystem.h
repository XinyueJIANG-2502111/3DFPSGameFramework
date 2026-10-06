#pragma once

#include "Engine/Effects/IVisualEffect.h"
#include <memory>
#include <vector>

// EN: A scene owns this system so effects cannot outlive their world.
//     unique_ptr gives each effect exactly one owner.
// JP: Scene が System を所有し、Effect が World より長く生存しないようにする。
//     unique_ptr によって Effect の所有者を一つに限定する。
class VisualEffectSystem
{
public:
    void Add(std::unique_ptr<IVisualEffect> effect);
    void Update(float deltaTime);
    void Render(Renderer& renderer) const;
    void Clear();

private:
    std::vector<std::unique_ptr<IVisualEffect>> m_effects;
};
