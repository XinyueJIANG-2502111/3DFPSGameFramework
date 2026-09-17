#pragma once

#include "Engine/Math/Vector3.h"

class Collider;

struct CollisionHit
{
    // EN: The other collider involved in the collision.
    //     This is a non-owning pointer.
    //
    // JP: Collision の相手となる Collider。
    //     所有権を持たないポインタ。
    const Collider* other = nullptr;

    // EN: Direction in which the querying collider should be moved
    //     to resolve the overlap.
    //
    // JP: 重なりを解消するために Query 側の Collider を
    //     移動させる方向。
    Vector3 normal{};

    // EN: Minimum distance required to separate the colliders
    //     along the collision normal.
    //
    // JP: Collision Normal の方向へ Collider を分離するために
    //     必要な最小距離。
    float penetration = 0.0f;
};