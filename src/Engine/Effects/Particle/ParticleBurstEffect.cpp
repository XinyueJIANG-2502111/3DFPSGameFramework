#include "Engine/Effects/Particle/ParticleBurstEffect.h"
#include "Engine/Rendering/Renderer.h"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <random>

ParticleBurstEffect::ParticleBurstEffect(const ParticleBurstDesc& desc)
    : m_color(desc.color)
{
    const int count = std::max(desc.particleCount, 0);
    const float speedMin = std::max(desc.speedMin, 0.0f);
    const float speedMax = std::max(desc.speedMax, speedMin);
    const float lifetimeMin = std::max(desc.lifetimeMin, 0.001f);
    const float lifetimeMax = std::max(desc.lifetimeMax, lifetimeMin);
    Vector3 direction = Normalize(desc.direction);
    if (direction.LengthSquared() == 0.0f)
    {
        direction = Vector3{ 0.0f, 1.0f, 0.0f };
    }
    // EN: A fixed seed makes the V1 sample reproducible without a global
    //     random service. Each burst still owns independent particles.
    // JP: 固定 Seed で V1 の検証を再現可能にし、Global Random Service を不要にする。
    //     各 Burst の Particle は独立して所有される。
    std::mt19937 random(27);
    std::uniform_real_distribution<float> unit(0.0f, 1.0f);
    std::uniform_real_distribution<float> speed(speedMin, speedMax);
    std::uniform_real_distribution<float> lifetime(lifetimeMin, lifetimeMax);
    m_particles.reserve(count);
    m_billboards.reserve(count);
    for (int i = 0; i < count; ++i)
    {
        const float z = unit(random) * 2.0f - 1.0f;
        const float angle = unit(random) * 2.0f * std::numbers::pi_v<float>;
        const float radius = std::sqrt(std::max(0.0f, 1.0f - z * z));
        const Vector3 spreadDirection{
            radius * std::cos(angle), radius * std::sin(angle), z
        };
        Vector3 velocityDirection = Normalize(
            direction + spreadDirection * std::max(desc.spread, 0.0f));
        if (velocityDirection.LengthSquared() == 0.0f)
        {
            velocityDirection = direction;
        }
        m_particles.push_back(Particle{
            desc.position, velocityDirection * speed(random),
            0.0f, lifetime(random),
            std::max(desc.startSize, 0.0f), std::max(desc.endSize, 0.0f)
        });
    }
    RebuildBillboards();
}

void ParticleBurstEffect::Update(float deltaTime)
{
    if (!std::isfinite(deltaTime) || deltaTime < 0.0f)
    {
        return;
    }
    for (auto& particle : m_particles)
    {
        particle.age += deltaTime;
        particle.position += particle.velocity * deltaTime;
    }
    std::erase_if(m_particles, [](const Particle& particle) {
        return particle.age >= particle.lifetime;
    });
    RebuildBillboards();
}

void ParticleBurstEffect::RebuildBillboards()
{
    m_billboards.clear();
    for (const auto& particle : m_particles)
    {
        const float t = std::clamp(particle.age / particle.lifetime, 0.0f, 1.0f);
        m_billboards.push_back(BillboardRenderData{
            particle.position,
            std::lerp(particle.startSize, particle.endSize, t),
            m_color, 1.0f - t
        });
    }
}

void ParticleBurstEffect::Render(Renderer& renderer) const
{
    // EN: Additive sparks avoid requiring transparent depth sorting in V1.
    //     Renderer receives only drawing data, never particle lifetime logic.
    // JP: V1 では加算合成の Spark を使い、透明描画の深度 Sort を不要にする。
    //     Renderer には描画データだけを渡し、Particle の寿命処理を持たせない。
    renderer.DrawBillboards(m_billboards, BillboardBlendMode::Additive);
}

bool ParticleBurstEffect::IsFinished() const
{
    return m_particles.empty();
}
