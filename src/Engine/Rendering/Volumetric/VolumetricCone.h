#pragma once

#include "Engine/Math/Vector3.h"

struct VolumetricCone
{
    // EN: World-space origin of the volumetric light cone.
    //
    // JP: Volumetric Light Cone の World Space Origin。
    Vector3 position{};

    // EN: World-space forward direction of the cone.
    //
    // JP: Cone が向いている World Space 上の Forward Direction。
    Vector3 direction{
        0.0f,
        0.0f,
        1.0f
    };

    // EN: Maximum visible length of the volumetric beam.
    //
    // JP: Volumetric Beam の最大表示距離。
    float range = 15.0f;

    // EN: Outer half-angle of the cone in radians.
    //
    // JP: Cone の Outer Half Angle。単位は Radian。
    float outerAngle = 0.5f;
};