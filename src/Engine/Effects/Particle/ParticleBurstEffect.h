#pragma once

#include "Engine/Effects/IVisualEffect.h"
#include "Engine/Effects/Particle/Particle.h"
#include "Engine/Rendering/Billboard/BillboardRenderData.h"
#include <vector>

struct ParticleBurstDesc
{
    Vector3 position{};
    Vector3 direction{ 0.0f, 1.0f, 0.0f };
    int particleCount = 16;
    float speedMin = 0.7f;
    float speedMax = 2.5f;
    float lifetimeMin = 0.3f;
    float lifetimeMax = 0.8f;
    float startSize = 0.1f;
    float endSize = 0.0f;
    Vector3 color{ 1.0f, 0.65f, 0.2f };
    float spread = 1.5f;
};

class ParticleBurstEffect final : public IVisualEffect
{
public:
    explicit ParticleBurstEffect(const ParticleBurstDesc& desc);
    void Update(float deltaTime) override;
    void Render(Renderer& renderer) const override;
    [[nodiscard]] bool IsFinished() const override;

private:
    void RebuildBillboards();
    std::vector<Particle> m_particles;
    std::vector<BillboardRenderData> m_billboards;
    Vector3 m_color;
};
