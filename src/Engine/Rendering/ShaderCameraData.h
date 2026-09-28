#pragma once

#include "Engine/Math/Vector3.h"

struct ShaderCameraData
{
    // EN: Camera position in world space.
    //
    // JP: World Space è„ÇÃ Camera PositionÅB
    Vector3 position{
        0.0f,
        0.0f,
        0.0f
    };

    float padding = 0.0f;
};