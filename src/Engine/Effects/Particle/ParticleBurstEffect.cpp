#include "Engine/Effects/Particle/ParticleBurstEffect.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/Texture/Texture.h"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <random>

ParticleBurstEffect::ParticleBurstEffect(const ParticleBurstDesc& desc)
    : m_color(desc.color)
    , m_blendMode(desc.blendMode)
    , m_texture(desc.texture)
{
    const int count = std::max(desc.particleCount, 0);
    const float speedMin = std::max(desc.speedMin, 0.0f);
    const float speedMax = std::max(desc.speedMax, speedMin);
    const float lifetimeMin = std::max(desc.lifetimeMin, 0.001f);
    const float lifetimeMax = std::max(desc.lifetimeMax, lifetimeMin);
    const float startWidth = std::max(desc.startWidth, 0.0f);
    const float endWidth = std::max(desc.endWidth, 0.0f);
    const float startHeight = std::max(desc.startHeight, 0.0f);
    const float endHeight = std::max(desc.endHeight, 0.0f);
    const float angularVelocityMin =
        std::min(desc.angularVelocityMin, desc.angularVelocityMax);
    const float angularVelocityMax =
        std::max(desc.angularVelocityMin, desc.angularVelocityMax);
    Vector3 direction = Normalize(desc.direction);
    if (direction.LengthSquared() == 0.0f)
    {
        direction = Vector3{ 0.0f, 1.0f, 0.0f };
    }
    // EN: A fixed seed makes the V1 sample reproducible without a global
    //     random service. Each burst still owns independent particles.
    // JP: 固定 Seed で V1 の検証を再現可能にし、Global Random Service を不要にする。
    //     各 Burst の Particle は独立して所有される。
    std::mt19937 random(desc.randomSeed);
    std::uniform_real_distribution<float> unit(0.0f, 1.0f);
    std::uniform_real_distribution<float> speed(speedMin, speedMax);
    std::uniform_real_distribution<float> lifetime(lifetimeMin, lifetimeMax);
    std::uniform_real_distribution<float> angularVelocity(
        angularVelocityMin,
        angularVelocityMax);
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
        Particle particle;
        particle.position = desc.position;
        particle.velocity = velocityDirection * speed(random);
        particle.acceleration = desc.acceleration;
        particle.lifetime = lifetime(random);
        particle.startWidth = startWidth;
        particle.endWidth = endWidth;
        particle.startHeight = startHeight;
        particle.endHeight = endHeight;
        particle.angularVelocity = angularVelocity(random);
        m_particles.push_back(particle);
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
        particle.velocity += particle.acceleration * deltaTime;
        particle.position += particle.velocity * deltaTime;
        particle.rotationRadians +=
            particle.angularVelocity * deltaTime;
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
        BillboardRenderData billboard;
        billboard.position = particle.position;
        billboard.width =
            std::lerp(particle.startWidth, particle.endWidth, t);
        billboard.height =
            std::lerp(particle.startHeight, particle.endHeight, t);
        billboard.rotationRadians = particle.rotationRadians;
        billboard.color = m_color;
        billboard.alpha = 1.0f - t;
        m_billboards.push_back(billboard);
    }
}

void ParticleBurstEffect::Render(Renderer& renderer) const
{
    // EN: The effect selects the batch blend mode and texture, while the
    //     renderer still owns all backend state and draw-call details.
    // JP: Effect は Batch の Blend Mode と Texture を選択し、Renderer は
    //     Backend State と Draw Call の詳細だけを管理する。
    BillboardDrawSettings settings;
    settings.blendMode = m_blendMode;
    settings.texture = m_texture.get();
    renderer.DrawBillboards(m_billboards, settings);
}

bool ParticleBurstEffect::IsFinished() const
{
    return m_particles.empty();
}
