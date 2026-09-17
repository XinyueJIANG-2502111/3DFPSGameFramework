#pragma once

#include "Engine/Physics/Collision/AABB.h"

bool Intersects(
    const AABB& a,
    const AABB& b);

bool Contains(
    const AABB& bounds,
    const Vector3& point);

// EN: Computes the minimum direction and distance required to move
//     A out of B. Returns false when the AABBs do not overlap.
//
// JP: A を B から分離するために必要な最小方向と距離を計算する。
//     AABB が重なっていない場合は false を返す。
bool ComputePenetration(
    const AABB& a,
    const AABB& b,
    Vector3& normal,
    float& penetration);