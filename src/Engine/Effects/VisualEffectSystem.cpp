#include "Engine/Effects/VisualEffectSystem.h"

#include <algorithm>
#include <cmath>
#include <utility>

void VisualEffectSystem::Add(std::unique_ptr<IVisualEffect> effect)
{
    if (effect && !effect->IsFinished())
    {
        m_effects.push_back(std::move(effect));
    }
}

void VisualEffectSystem::Update(float deltaTime)
{
    if (!std::isfinite(deltaTime) || deltaTime < 0.0f)
    {
        return;
    }
    for (auto& effect : m_effects)
    {
        effect->Update(deltaTime);
    }
    std::erase_if(m_effects, [](const auto& effect) {
        return effect->IsFinished();
    });
}

void VisualEffectSystem::Render(Renderer& renderer) const
{
    for (const auto& effect : m_effects)
    {
        effect->Render(renderer);
    }
}

void VisualEffectSystem::Clear()
{
    m_effects.clear();
}
