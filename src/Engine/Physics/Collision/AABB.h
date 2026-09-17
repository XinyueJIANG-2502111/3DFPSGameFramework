#pragma once

#include "Engine/Math/Vector3.h"

#include <algorithm>

struct AABB
{
    Vector3 min{};
    Vector3 max{};

    constexpr AABB() = default;

    // EN: Constructs a valid axis-aligned bounding box from two
    //     arbitrary corner points. The component-wise minimum and
    //     maximum are selected automatically.
    //
    // JP: 任意の2つの頂点から有効な AABB を構築する。
    //     各成分の最小値・最大値は自動的に選択される。
    AABB(
        const Vector3& pointA,
        const Vector3& pointB)
        : min{
            std::min(pointA.x, pointB.x),
            std::min(pointA.y, pointB.y),
            std::min(pointA.z, pointB.z)
        }
        , max{
            std::max(pointA.x, pointB.x),
            std::max(pointA.y, pointB.y),
            std::max(pointA.z, pointB.z)
        }
    {
    }

    // EN: Returns the full size of the box along each world axis.
    //
    // JP: 各ワールド軸方向における Box 全体のサイズを返す。
    constexpr Vector3 GetSize() const
    {
        return max - min;
    }

    // EN: Returns the geometric center of the box.
    //
    // JP: Box の幾何学的な中心位置を返す。
    constexpr Vector3 GetCenter() const
    {
        return (min + max) * 0.5f;
    }

    // EN: Returns a copy of this AABB translated by the specified
    //     world-space displacement.
    //
    // JP: 指定したワールド空間上の移動量だけ平行移動した
    //     AABB のコピーを返す。
    AABB Translated(
        const Vector3& displacement) const
    {
        return AABB{
            min + displacement,
            max + displacement
        };
    }
};