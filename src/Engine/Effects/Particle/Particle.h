#pragma once

#include "Engine/Math/Vector3.h"

struct Particle
{
    Vector3 position{};
    Vector3 velocity{};
    Vector3 acceleration{};

    float age = 0.0f;
    float lifetime = 1.0f;

    float startWidth = 0.1f;
    float endWidth = 0.0f;
    float startHeight = 0.1f;
    float endHeight = 0.0f;

    float rotationRadians = 0.0f;
    float angularVelocity = 0.0f;
};
