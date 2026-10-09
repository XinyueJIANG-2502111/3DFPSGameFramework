#pragma once

class Renderer;

// EN: Persistent state-driven screen VFX for low player health. It is kept
//     outside VisualEffectSystem because it is not a short-lived world effect.
// JP: Persistent screen VFX stays separate from VisualEffectSystem.
class LowHealthScreenEffect final
{
public:
    void SetHealthRatio(float ratio);
    void Update(float deltaTime);
    void Render(Renderer& renderer) const;

private:
    float CalculateIntensity() const;

    float m_healthRatio = 0.0f;
    float m_elapsedTime = 0.0f;
};
