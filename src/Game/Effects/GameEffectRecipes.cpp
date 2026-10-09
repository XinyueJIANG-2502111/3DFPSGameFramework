#include "Game/Effects/GameEffectRecipes.h"

#include "Engine/Math/Vector3.h"

#include <utility>

ParticleBurstDesc MakeMetalSparkBurst(
    const Vector3& position,
    const Vector3& normal,
    std::shared_ptr<Texture> texture,
    std::uint32_t seed)
{
    Vector3 safeNormal = Normalize(normal);
    if (safeNormal.LengthSquared() == 0.0f)
    {
        safeNormal = Vector3{ 0.0f, 1.0f, 0.0f };
    }

    ParticleBurstDesc result;
    result.position = position;
    result.direction = safeNormal;
    result.particleCount = 18;
    result.speedMin = 3.0f;
    result.speedMax = 8.0f;
    result.lifetimeMin = 0.10f;
    result.lifetimeMax = 0.40f;
    result.acceleration = Vector3{ 0.0f, -4.5f, 0.0f };
    result.startWidth = 0.02f;
    result.endWidth = 0.001f;
    result.startHeight = 0.16f;
    result.endHeight = 0.005f;
    result.angularVelocityMin = -6.0f;
    result.angularVelocityMax = 6.0f;
    result.blendMode = BillboardBlendMode::Additive;
    result.texture = std::move(texture);
    result.color = Vector3{ 1.0f, 0.78f, 0.25f };
    result.spread = 0.45f;
    result.randomSeed = seed;

    // EN: Keep sparks narrow and bright so the recipe reads as metal impact,
    //     while ParticleBurstEffect remains reusable for other effects.
    // JP: �߂��� Metal Impact �Ɍ����A���q��細く明るく調整する。
    //     ParticleBurstEffect �̋��p���𖫂��Ȃ��悤�ɂ���B
    return result;
}

ParticleBurstDesc MakeBloodSprayBurst(
    const Vector3& position,
    const Vector3& direction,
    std::shared_ptr<Texture> texture,
    std::uint32_t seed)
{
    Vector3 safeDirection = Normalize(direction);
    if (safeDirection.LengthSquared() == 0.0f)
    {
        safeDirection = Vector3{ 0.0f, 0.0f, 1.0f };
    }

    ParticleBurstDesc result;
    result.position = position;
    result.direction = safeDirection;
    result.particleCount = 30;
    result.speedMin = 1.5f;
    result.speedMax = 5.0f;
    result.lifetimeMin = 0.35f;
    result.lifetimeMax = 1.0f;
    result.acceleration = Vector3{ 0.0f, -9.8f, 0.0f };
    result.startWidth = 0.10f;
    result.endWidth = 0.015f;
    result.startHeight = 0.12f;
    result.endHeight = 0.01f;
    result.angularVelocityMin = -2.5f;
    result.angularVelocityMax = 2.5f;
    result.blendMode = BillboardBlendMode::Alpha;
    result.texture = std::move(texture);
    result.color = Vector3{ 0.35f, 0.015f, 0.02f };
    result.spread = 0.75f;
    result.randomSeed = seed;

    // EN: Blood remains an airborne alpha-blended burst. Surface decals and
    //     persistent stains belong to a later gameplay system.
    // JP: Blood �͋󒆂݂̂� Alpha Blend �̓���Ƃ��A�ʒu�Ɏc���V�X�e���͌�񂪂���B
    return result;
}
