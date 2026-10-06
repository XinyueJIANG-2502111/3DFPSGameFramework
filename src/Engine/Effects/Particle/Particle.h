#pragma once

#include "Engine/Math/Vector3.h"

struct Particle
{
    Vector3 position{};
    Vector3 velocity{};
    float age = 0.0f;
    float lifetime = 1.0f;
    float startSize = 0.1f;
    float endSize = 0.0f;
};
