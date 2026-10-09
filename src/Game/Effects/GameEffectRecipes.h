#pragma once

#include "Engine/Effects/Particle/ParticleBurstEffect.h"

#include <cstdint>
#include <memory>

class Texture;

// EN: Creates the gameplay tuning for a short metal-impact spark burst.
//     The engine effect remains generic and owns no game-specific values.
// JP: ���I�ȃ��^���̒��� Metal Spark Burst �� Gameplay �Œ��肷��B
//     Engine Effect �ɃQ�[���ŗL�̒l��ێ������Ȃ��B
ParticleBurstDesc MakeMetalSparkBurst(
    const Vector3& position,
    const Vector3& normal,
    std::shared_ptr<Texture> texture,
    std::uint32_t seed);

// EN: Creates a gravity-driven airborne blood spray using alpha blending.
// JP: Alpha Blend ��g�p����重力���̓��� Blood Spray �� Gameplay �Œ��肷��B
ParticleBurstDesc MakeBloodSprayBurst(
    const Vector3& position,
    const Vector3& direction,
    std::shared_ptr<Texture> texture,
    std::uint32_t seed);
